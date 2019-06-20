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
    buf_ += bytes;
    MaybeSatisfyConsumers();
  });
}

void IOManager::Write(const IOProducer& req) {
  switch (req.producer_case()) {
    case IOProducer::kLiteral:
      process_->write(req.literal().literal());
      break;
    case IOProducer::kDecimalNumber: {
      auto data = absl::StrCat(req.decimal_number().number());
      process_->write(data);
      break;
    }
    case IOProducer::kLittleEndianNumber: {
      int64_t data = req.little_endian_number().number();
      std::string_view marshalled(reinterpret_cast<char*>(&data),
                                  req.little_endian_number().width());
      process_->write(marshalled);
      break;
    }
    case IOProducer::PRODUCER_NOT_SET:
      break;
  }
}

namespace {
std::unique_ptr<ReadConsumer> ParseConsumer(const IOConsumer& proto) {
  std::unique_ptr<ReadConsumer> consumer;
  switch (proto.consumer_case()) {
    case IOConsumer::kAny:
      consumer = std::make_unique<ReadConsumer_Any>();
      break;
    case IOConsumer::kLiteral:
      consumer =
          std::make_unique<ReadConsumer_Literal>(proto.literal().literal());
      break;
    case IOConsumer::kNumber:
      consumer = std::make_unique<ReadConsumer_Number>();
      break;
    case IOConsumer::kLine:
      consumer = std::make_unique<ReadConsumer_Line>(
          ParseConsumer(proto.line().predicate()));
      break;
    case IOConsumer::kOneof: {
      std::vector<std::unique_ptr<ReadConsumer>> alts;
      for (const auto& inner : proto.oneof().consumers()) {
        alts.push_back(ParseConsumer(inner));
      }
      consumer = std::make_unique<ReadConsumer_Oneof>(std::move(alts));
      break;
    }
    case IOConsumer::kChain: {
      auto chain = std::make_unique<ReadConsumer_Chain>();
      for (const auto& inner : proto.chain().consumers()) {
        chain->Add(ParseConsumer(inner));
      }
      consumer = std::move(chain);
    }
    default:
      LOG(FATAL) << "Unimplemented consumer type. Proto: "
                 << proto.DebugString();
  }
  return consumer;
}
}  // namespace

void IOManager::Read(const IOConsumer& req) {
  absl::MutexLock l(&mutex_);
  ReadConsumer_Chain* chain = nullptr;
  if (pending_reads_.empty() || pending_reads_.back() != req.chain_id()) {
    if (!pending_reads_.empty()) {
      chains_.at(pending_reads_.back())->Seal();
    }
    chains_[req.chain_id()] = std::make_unique<ReadConsumer_Chain>();
    pending_reads_.push_back(req.chain_id());
    chain = chains_.at(req.chain_id()).get();
  } else {
    chain = chains_.at(pending_reads_.back()).get();
  }
  chain->Add(ParseConsumer(req));
  MaybeSatisfyConsumers();
}

void IOManager::MaybeSatisfyConsumers() {
  while (!pending_reads_.empty()) {
    auto* chain = chains_.at(pending_reads_.front()).get();
    if (!chain->sealed()) {
      return;
    }
    chain->Reset();
    chain->AppendAvailableData(buf_);
    std::string_view remaining;
    if (chain->NextMatch(&remaining)) {
      buf_ = std::string(remaining);
      pending_reads_.pop_front();
    } else {
      return;
    }
  }
}

void IOManager::ReadSync(int chain_id, IOReadResult* result) {
  {
    absl::WriterMutexLock l(&mutex_);
    chains_.at(chain_id)->Seal();
    MaybeSatisfyConsumers();
  }
  {
    absl::ReaderMutexLock l(&mutex_);
    struct AwaitCondition {
      int chain_id;
      const IOManager* manager;
    } condition{chain_id, this};
    mutex_.Await(absl::Condition(
        +[](AwaitCondition* condition)
             SHARED_LOCKS_REQUIRED(condition->manager->mutex_) {
               auto chain =
                   condition->manager->chains_.at(condition->chain_id).get();
               return chain->valid() && chain->sealed();
             },
        &condition));
    return chains_.at(chain_id)->Get(result);
  }
}

void IOManager::OnUIUpdate(
    const std::function<bool(const UIIODataUpdate*)>& callback) {
  // TODO: Implement.
}