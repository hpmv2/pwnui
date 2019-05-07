#include <regex>
#include "io-manager.h"
#include "glog/logging.h"

IOManager::IOManager(Process* process) : process_(process) {
  process_->read([this](std::string_view bytes) {
    absl::MutexLock l(&mutex_);
    while (!pending_reads_.empty()) {
      ProcessDataForFrontReader(&bytes);
    }
    if (!bytes.empty()) {
      buffer_.emplace_back(bytes);
    }
  });
}

void IOManager::Write(const IOWriteRequest& req) {
  // TODO: add IOChunks.
  for (const auto& element : req.elements()) {
    switch (element.write_case()) {
      case IOWriteElement::kData:
        process_->write(element.data());
        break;
      case IOWriteElement::kDecimalInteger:
        process_->write(absl::StrCat(element.decimal_integer()));
        break;
      case IOWriteElement::kLittleEndianInteger: {
        int64_t data = element.little_endian_integer().integer();
        std::string_view marshalled(reinterpret_cast<char*>(&data),
                                    element.little_endian_integer().width());
        process_->write(marshalled);
        break;
      }
      case IOWriteElement::WRITE_NOT_SET:break;
    }
  }
}

void IOManager::Read(const IOReadRequest& req, IOReadResponse* resp) {
  // TODO: add IOChunks.
  absl::MutexLock l(&mutex_);
  for (const auto& element : req.elements()) {
    switch (element.read_case()) {
      case IOReadElement::kRegex: {
        std::vector<IOElementType> groups;
        for (int i = 0; i < element.regex().groups_size(); i++) {
          groups.push_back(element.regex().groups(i));
        }
        pending_reads_.push(std::make_unique<ElementReadState_ReadRegex>(
            element.regex().regex(), groups));
      }
      case IOReadElement::kLittleEndian32:
        pending_reads_.push(
            std::make_unique<ElementReadState_ReadLittleEndian32>());
        break;
      case IOReadElement::READ_NOT_SET:break;
    }
  }

  struct Args {
    IOManager* me;
    int n;
  } args{this, req.elements_size()};

  absl::Condition condition(
      +[](Args* args) { return args->me->read_results_.size() >= args->n; },
      &args);

  while (!buffer_.empty() && !condition.Eval()) {
    std::string_view data = buffer_.front();
    ProcessDataForFrontReader(&data);
    if (!data.empty()) {
      std::string partial(data);
      buffer_.pop_front();
      buffer_.push_front(std::move(partial));
    }
  }

  mutex_.Await(condition);
  for (int i = 0; i < req.elements_size(); i++) {
    *resp->add_elements() = *read_results_.front();
    read_results_.pop();
  }
}

void IOManager::ProcessDataForFrontReader(std::string_view* data) {
  std::vector<IOElement*> elements; // TODO: use this.
  auto result =
      pending_reads_.front()->ProcessAvailableData(data, &elements, &arena_);
  if (result) {
    read_results_.push(result);
    pending_reads_.pop();
  }
}

IOReadResultElement* ElementReadState_ReadLittleEndian32::ProcessAvailableData(
    std::string_view* data, std::vector<IOElement*>* elements,
    google::protobuf::Arena* arena) {
  int n = (int) std::min(data->size(), 4 - current_.size());
  if (n == 0) {
    return nullptr;
  }
  current_ += data->substr(0, n);
  *data = data->substr(n);
  auto element = google::protobuf::Arena::CreateMessage<IOElement>(arena);
  element->set_data(current_);
  element->set_pending(true);
  element->set_type(IOET_HEX32);
  elements->push_back(element);
  if (current_.size() == 4) {
    element->set_pending(false);
    auto result =
        google::protobuf::Arena::CreateMessage<IOReadResultElement>(arena);
    result->set_data(current_);
    return result;
  }
  return nullptr;
}

IOReadResultElement* ElementReadState_ReadRegex::ProcessAvailableData(
    std::string_view* data, std::vector<IOElement*>* elements,
    google::protobuf::Arena* arena) {

  current_ += *data;
  std::smatch m;
  if (std::regex_search(current_, m, re_)) {
    LOG(INFO) << "Search found";
    auto result =
        google::protobuf::Arena::CreateMessage<IOReadResultElement>(arena);
    int prefix = m.prefix().length();
    int suffix = m.suffix().length();
    IOElement* element = nullptr;
    if (prefix) {
      element = google::protobuf::Arena::CreateMessage<IOElement>(arena);
      element->set_data(current_.substr(0, prefix));
      element->set_type(IOET_UNINTERESTING);
      elements->push_back(element);
    }
    LOG(INFO) << __LINE__;
    int ptr = 0;
    for (int i = 1; i < m.size(); i++) {
      LOG(INFO) << __LINE__;
      std::ssub_match sub_match = m[i];
      if (ptr != m.position(i)) {
        element = google::protobuf::Arena::CreateMessage<IOElement>(arena);
        element->set_data(current_.substr(prefix + ptr, m.position(i) - ptr));
        element->set_type(IOET_RAW);
        elements->push_back(element);
      }
      LOG(INFO) << __LINE__;
      ptr = m.position(i);
      element = google::protobuf::Arena::CreateMessage<IOElement>(arena);
      element->set_data(current_.substr(prefix + ptr, m.length(i)));
      element->set_type(groups_[i]); // TODO(hpmv): error check.
      elements->push_back(element);
      ptr += m.length(i);
      result->add_groups(sub_match.str());
      LOG(INFO) << __LINE__;
    }
    if (current_.size() - suffix > ptr) {
      element = google::protobuf::Arena::CreateMessage<IOElement>(arena);
      element->set_data(current_.substr(ptr, current_.size() - suffix - ptr));
      element->set_type(IOET_RAW);
      elements->push_back(element);
    }
    LOG(INFO) << __LINE__;
    *data = data->substr(data->size() - suffix);
    result->set_data(current_.substr(0, current_.size() - suffix));
    LOG(INFO) << "Returning " << result->DebugString();
    return result;
  } else {
    auto element = google::protobuf::Arena::CreateMessage<IOElement>(arena);
    element->set_data(current_);
    element->set_pending(true);
    element->set_type(IOET_UNINTERESTING);
    elements->push_back(element);
    *data = data->substr(data->size());
    return nullptr;
  }
}