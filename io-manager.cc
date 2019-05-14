#include "io-manager.h"
#include <regex>
#include "glog/logging.h"

IOManager::IOManager(Process* process) : process_(process) {
  process_->read([this](std::string_view bytes, bool is_stderr) {
    if (is_stderr) {
      // TODO: What to do with stderr?
      return;
    }
    absl::MutexLock l(&mutex_);
    while (!pending_reads_.empty()) {
      ProcessDataForFrontReader(&bytes);
    }
    if (!bytes.empty()) {
      auto chunk_id = next_chunk_id_++;
      buffer_.push_back(PendingDataState{chunk_id, std::string(bytes)});
      UIIODataUpdate* update = NewUpdate();
      auto op = update->mutable_append_operation();
      op->set_source(UIIOOperation::STDOUT);
      auto chunk = op->add_chunks();
      chunk->set_chunk_id(chunk_id);
      IOElementAdder adder(chunk->mutable_elements());
      adder.Add(std::string(bytes), IOET_UNPROCESSED);
      AddUIUpdate(update);
    }
  });
}

void IOManager::Write(const IOWriteRequest& req) {
  UIIODataUpdate* update = NewUpdate();
  auto op = update->mutable_append_operation();
  op->set_source(UIIOOperation::STDIN);
  for (const auto& element : req.chunks()) {
    auto chunk = op->add_chunks();
    chunk->set_chunk_id(next_chunk_id_++);
    IOElementAdder adder(chunk->mutable_elements());
    switch (element.write_case()) {
      case IOWriteChunk::kData:
        process_->write(element.data());
        adder.Add(element.data(), IOET_RAW);
        break;
      case IOWriteChunk::kDecimalInteger: {
        auto data = absl::StrCat(element.decimal_integer());
        process_->write(data);
        adder.Add(data, IOET_DECIMAL32);
        break;
      }
      case IOWriteChunk::kLittleEndianInteger: {
        int64_t data = element.little_endian_integer().integer();
        std::string_view marshalled(reinterpret_cast<char*>(&data),
                                    element.little_endian_integer().width());
        process_->write(marshalled);
        adder.Add(std::string(marshalled), IOET_HEX32);
        break;
      }
      case IOWriteChunk::WRITE_NOT_SET:
        break;
    }
  }
  AddUIUpdate(update);
}

void IOManager::Read(const IOReadRequest& req, IOReadResponse* resp) {
  if (req.chunks_size() == 0) {
    return;
  }
  absl::MutexLock l(&mutex_);
  UIIODataUpdate* update = NewUpdate();
  auto op = update->mutable_append_operation();
  op->set_source(UIIOOperation::STDOUT);
  auto chunks = op->mutable_chunks();
  for (const auto& element : req.chunks()) {
    int chunk_id = next_chunk_id_++;
    switch (element.read_case()) {
      case IOReadChunk::kRegex: {
        std::vector<IOElementType> groups;
        for (int i = 0; i < element.regex().groups_size(); i++) {
          groups.push_back(element.regex().groups(i));
        }
        pending_reads_.push(std::make_unique<ChunkReadState_ReadRegex>(
            chunk_id, element.regex().regex(), groups));
        break;
      }
      case IOReadChunk::kLittleEndian32: {
        pending_reads_.push(
            std::make_unique<ChunkReadState_ReadLittleEndian32>(chunk_id));
        break;
      }
      case IOReadChunk::READ_NOT_SET:
        break;
    }
    std::string_view empty;
    auto chunk = chunks->Add();
    chunk->set_chunk_id(chunk_id);
    IOElementAdder elements(chunk->mutable_elements());
    pending_reads_.back()->ProcessAvailableData(&empty, &elements, &arena_);
  }
  AddUIUpdate(update);

  struct Args {
    IOManager* me;
    int n;
  } args{this, req.chunks_size()};

  absl::Condition condition(
      +[](Args* args) { return args->me->read_results_.size() >= args->n; },
      &args);

  while (!buffer_.empty() && !condition.Eval()) {
    PendingDataState data = std::move(buffer_.front());
    buffer_.pop_front();
    std::string_view data_view = data.data;
    ProcessDataForFrontReader(&data_view);
    UIIODataUpdate* update = NewUpdate();
    auto update_chunk = update->mutable_update_chunk();
    update_chunk->set_chunk_id(data.chunk_id);
    if (!data_view.empty()) {
      data.data = std::string(data_view);
      IOElementAdder adder(update_chunk->mutable_elements());
      adder.Add(data.data, IOET_UNPROCESSED);
      AddUIUpdate(update);
      buffer_.push_front(std::move(data));
    } else {
      update_chunk->set_remove(true);
      AddUIUpdate(update);
    }
  }

  mutex_.Await(condition);
  for (int i = 0; i < req.chunks_size(); i++) {
    *resp->add_chunks() = *read_results_.front();
    read_results_.pop();
  }
}

void IOManager::ProcessDataForFrontReader(std::string_view* data) {
  auto state = pending_reads_.front().get();
  UIIODataUpdate* update = NewUpdate();
  auto chunk_update = update->mutable_update_chunk();
  chunk_update->set_chunk_id(state->chunk_id());
  IOElementAdder elements(chunk_update->mutable_elements());
  auto result = state->ProcessAvailableData(data, &elements, &arena_);
  AddUIUpdate(update);
  if (result) {
    read_results_.push(result);
    pending_reads_.pop();
  }
}

void IOManager::AddUIUpdate(const UIIODataUpdate* update) {
  LOG(INFO) << "IO Update: " << update->DebugString();
  absl::MutexLock l(&updates_mutex_);
  all_updates_.push_back(update);
}

void IOManager::OnUIUpdate(
    const std::function<bool(const UIIODataUpdate*)>& callback) {
  struct Args {
    IOManager* me;
    int n;
  } args = {this, 0};
  absl::Condition condition(
      +[](Args* args) { return args->me->all_updates_.size() > args->n; },
      &args);
  while (true) {
    updates_mutex_.ReaderLockWhenWithTimeout(condition, absl::Seconds(1));
    if (args.n < all_updates_.size()) {
      for (; args.n < all_updates_.size(); args.n++) {
        if (!callback(all_updates_[args.n])) {
          updates_mutex_.ReaderUnlock();
          goto done;
        }
      }
    } else {
      if (!callback(nullptr)) {
        updates_mutex_.ReaderUnlock();
        goto done;
      }
    }
    updates_mutex_.ReaderUnlock();
  }
done:;
}

IOReadChunkResult* ChunkReadState_ReadLittleEndian32::ProcessAvailableData(
    std::string_view* data, IOElementAdder* elements,
    google::protobuf::Arena* arena) {
  int n = (int)std::min(data->size(), 4 - current_.size());
  current_ += data->substr(0, n);
  *data = data->substr(n);
  if (current_.size() == 4) {
    elements->Add(current_, IOET_HEX32);
    auto result =
        google::protobuf::Arena::CreateMessage<IOReadChunkResult>(arena);
    result->set_data(current_);
    return result;
  } else {
    elements->AddPending(current_, IOET_HEX32);
    return nullptr;
  }
}

IOReadChunkResult* ChunkReadState_ReadRegex::ProcessAvailableData(
    std::string_view* data, IOElementAdder* elements,
    google::protobuf::Arena* arena) {
  current_ += *data;
  std::smatch m;
  if (std::regex_search(current_, m, re_)) {
    auto result =
        google::protobuf::Arena::CreateMessage<IOReadChunkResult>(arena);
    int prefix = m.prefix().length();
    int suffix = m.suffix().length();
    if (prefix) {
      elements->Add(current_.substr(0, prefix), IOET_UNINTERESTING);
    }
    int ptr = prefix;
    for (int i = 1; i < m.size(); i++) {
      std::ssub_match sub_match = m[i];
      if (ptr != m.position(i)) {
        elements->Add(current_.substr(ptr, m.position(i) - ptr), IOET_RAW);
      }
      ptr = m.position(i);
      auto type = i - 1 < groups_.size() ? groups_[i - 1] : IOET_RAW;
      elements->Add(current_.substr(ptr, m.length(i)), type);
      ptr += m.length(i);
      result->add_groups(sub_match.str());
    }
    if (current_.size() - suffix > ptr) {
      elements->Add(current_.substr(ptr, current_.size() - suffix - ptr),
                    IOET_RAW);
    }
    *data = data->substr(data->size() - suffix);
    result->set_data(current_.substr(0, current_.size() - suffix));
    return result;
  } else {
    elements->AddPending(current_, IOET_UNINTERESTING);
    *data = data->substr(data->size());
    return nullptr;
  }
}
