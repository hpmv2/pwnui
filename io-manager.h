#ifndef IO_MANAGER_H
#define IO_MANAGER_H

#include <queue>
#include <regex>
#include "absl/container/flat_hash_map.h"
#include "absl/strings/strip.h"
#include "absl/synchronization/mutex.h"
#include "glog/logging.h"
#include "google/protobuf/arena.h"
#include "process.h"
#include "service.pb.h"

class ReadConsumer {
 public:
  virtual ~ReadConsumer() = default;

  // Append data to the consumer's internal matching buffer.
  virtual void SetData(std::string_view data) = 0;

  // Advance to the next prefix match, returning false if the next match does
  // not exist. If false is returned, the current state is unaffected.
  virtual bool NextMatch(std::string_view* remaining) = 0;

  // Returns true if this consumer might return true on NextMatch.
  virtual bool MayHaveMoreMatches() = 0;

  // Gets the current match as a proto.
  virtual void Get(IOReadResult* result) = 0;
};

class ReadConsumer_Any : public ReadConsumer {
 public:
  void SetData(std::string_view data) override {
    buf_ = data;
    ptr_ = -1;
  }

  bool NextMatch(std::string_view* remaining) override {
    if (ptr_ >= static_cast<int>(buf_.length())) {
      return false;
    }
    ptr_++;
    *remaining = std::string_view(buf_).substr(ptr_);
    return true;
  }

  bool MayHaveMoreMatches() override {
    return ptr_ >= static_cast<int>(buf_.length());
  }

  void Get(IOReadResult* result) override {
    result->mutable_any();
    if (ptr_ >= 0) {
      result->set_data(buf_.substr(0, ptr_));
      result->set_chars_consumed(ptr_);
    }
  }

 private:
  std::string buf_;
  int ptr_ = -1;
};

class ReadConsumer_Literal : public ReadConsumer {
 public:
  ReadConsumer_Literal(std::string expected) : expected_(std::move(expected)) {}

  void SetData(std::string_view data) override {
    buf_ = data;
    matched_.reset();
  }

  bool MayHaveMoreMatches() override { return !matched_; }

  bool NextMatch(std::string_view* remaining) override {
    if (matched_) {
      return false;
    }
    *remaining = buf_;
    matched_ = absl::ConsumePrefix(remaining, expected_);
    return *matched_;
  }

  void Get(IOReadResult* result) override {
    result->mutable_literal();
    if (matched_ && *matched_) {
      result->set_data(expected_);
      result->set_chars_consumed(expected_.size());
    }
  }

 private:
  std::string buf_;
  const std::string expected_;
  std::optional<bool> matched_;
};

class ReadConsumer_Number : public ReadConsumer {
 public:
  void SetData(std::string_view data) override {
    buf_ = data;
    ptr_.reset();
  }

  bool NextMatch(std::string_view* remaining) override {
    if (ptr_) return false;
    for (int i = 0; i < buf_.size(); i++) {
      if (buf_[i] >= '0' && buf_[i] <= '9') continue;
      ptr_ = i;
      *remaining = std::string_view(buf_).substr(*ptr_);
      return i > 0;
    }
    return false;
  }

  bool MayHaveMoreMatches() override { return !ptr_; }

  void Get(IOReadResult* result) override {
    result->mutable_number();
    if (ptr_) {
      result->set_data(buf_.substr(0, *ptr_));
      result->set_chars_consumed(*ptr_);
    }
  }

 private:
  std::string buf_;
  std::optional<int> ptr_;
};

class ReadConsumer_Line : public ReadConsumer {
 public:
  explicit ReadConsumer_Line(std::unique_ptr<ReadConsumer> inner)
      : inner_(std::move(inner)) {
    if (!inner_) {
      inner_ = std::make_unique<ReadConsumer_Any>();
    }
  }

  void SetData(std::string_view data) override {
    buf_ = data;
    ptr_.reset();
  }

  bool NextMatch(std::string_view* remaining) override {
    if (ptr_) return false;
    auto nl = buf_.find('\n');
    if (nl != std::string::npos) {
      inner_->SetData(buf_.substr(0, nl));
      std::string_view inner_rem;
      while (inner_->NextMatch(&inner_rem)) {
        if (inner_rem.empty()) {
          ptr_ = nl + 1;
          *remaining = std::string_view(buf_).substr(*ptr_);
          return true;
        }
      }
      ptr_ = 0;
    }
    return false;
  }

  bool MayHaveMoreMatches() override { return !ptr_; }

  void Get(IOReadResult* result) override {
    if (ptr_) {
      result->set_data(buf_.substr(0, *ptr_));
      inner_->Get(result->mutable_line()->mutable_inner());
      result->set_chars_consumed(*ptr_);
    } else {
      result->mutable_line();
    }
  }

 private:
  std::string buf_;
  std::optional<int> ptr_;
  std::unique_ptr<ReadConsumer> inner_;
};

class ReadConsumer_NChars : public ReadConsumer {
 public:
  ReadConsumer_NChars(int min, int max) : min_(min), max_(max) {}

  void SetData(std::string_view data) override {
    buf_ = data;
    ptr_ = -1;
  }

  bool NextMatch(std::string_view* remaining) override {
    if (ptr_ < min_) {
      if (buf_.length() < min_) {
        ptr_ = buf_.length();
        return false;
      }
      ptr_ = min_;
      *remaining = std::string_view(buf_).substr(ptr_);
      return true;
    }
    if (ptr_ < buf_.length() && ptr_ < max_) {
      ptr_++;
      *remaining = std::string_view(buf_).substr(ptr_);
      return true;
    }
    return false;
  }

  bool MayHaveMoreMatches() override {
    return ptr_ < max_ && ptr_ < buf_.length();
  }

  void Get(IOReadResult* result) override {
    result->mutable_nchars();
    if (ptr_ >= 0) {
      result->set_data(buf_.substr(0, ptr_));
      result->set_chars_consumed(ptr_);
    }
  }

 private:
  const int min_;
  const int max_;
  std::string buf_;
  int ptr_ = -1;
};

class ReadConsumer_Oneof : public ReadConsumer {
 public:
  ReadConsumer_Oneof(std::vector<std::unique_ptr<ReadConsumer>> alts)
      : alts_(std::move(alts)) {}

  void SetData(std::string_view data) override {
    for (auto& alt : alts_) {
      alt->SetData(data);
    }
    alt_ = 0;
  }
  bool MayHaveMoreMatches() override {
    for (auto& alt : alts_) {
      if (alt->MayHaveMoreMatches()) {
        return true;
      }
    }
    return false;
  }
  bool NextMatch(std::string_view* remaining) override {
    for (int i = 0; i < alts_.size(); i++) {
      auto& alt = alts_[i];
      if (alt->MayHaveMoreMatches()) {
        if (alt->NextMatch(remaining)) {
          alt_ = i;
          return true;
        }
      }
    }
    return false;
  }

  void Get(IOReadResult* result) override {
    result->mutable_oneof()->set_index(alt_);
    alts_[alt_]->Get(result->mutable_oneof()->mutable_inner());
    result->set_data(result->oneof().inner().data());
    result->set_chars_consumed(result->oneof().inner().chars_consumed());
  }

 private:
  std::vector<std::unique_ptr<ReadConsumer>> alts_;
  int alt_;
};

class ReadConsumer_Chain : public ReadConsumer {
 public:
  void Add(std::unique_ptr<ReadConsumer> element) {
    LOG_IF(FATAL, sealed_) << "Cannot add after sealing.";
    elements_.push_back(std::move(element));
  }

  void Seal() { sealed_ = true; }
  bool sealed() const { return sealed_; }

  void SetData(std::string_view data) override {
    elements_[0]->SetData(data);
    valid_prefix_ = 0;
  }

  bool MayHaveMoreMatches() override {
    if (valid_prefix_ < elements_.size()) return true;
    for (auto& ele : elements_) {
      if (ele->MayHaveMoreMatches()) {
        return true;
      }
    }
    return false;
  }

  bool NextMatch(std::string_view* remaining) override {
    int i = valid() ? elements_.size() - 1 : 0;
    while (true) {
      std::string_view rem;
      if (elements_[i]->NextMatch(&rem)) {
        i++;
        valid_prefix_ = i;
        if (i == elements_.size()) {
          *remaining = rem;
          return true;
        } else {
          elements_[i]->SetData(rem);
        }
      } else {
        i--;
        if (i == -1) {
          return false;
        }
      }
    }
  }

  void Get(IOReadResult* result) override {
    for (int i = 0; i < valid_prefix_; i++) {
      elements_[i]->Get(result->mutable_chain()->add_inner());
    }
    std::string data;
    for (const auto& inner : result->chain().inner()) {
      data += inner.data().substr(0, inner.chars_consumed());
    }
    result->set_data(data);
    result->set_chars_consumed(data.size());
  }

  bool valid() const { return valid_prefix_ == elements_.size(); }

 private:
  std::vector<std::unique_ptr<ReadConsumer>> elements_;
  bool sealed_ = false;
  int valid_prefix_ = 0;
  std::string buf_;
};

class ReadConsumer_Peek : public ReadConsumer {
 public:
  ReadConsumer_Peek(std::unique_ptr<ReadConsumer> inner)
      : inner_(std::move(inner)) {}
  void SetData(std::string_view data) override {
    data_ = data;
    inner_->SetData(data);
  }

  void Get(IOReadResult* result) override {
    inner_->Get(result->mutable_peek()->mutable_inner());
    result->set_data(result->peek().inner().data());
    result->set_chars_consumed(0);
  }

  bool MayHaveMoreMatches() override { return inner_->MayHaveMoreMatches(); }

  bool NextMatch(std::string_view* remaining) override {
    std::string_view rem;
    if (inner_->NextMatch(&rem)) {
      *remaining = data_;
      return true;
    }
    return false;
  }

 private:
  std::unique_ptr<ReadConsumer> inner_;
  std::string data_;
};

// Manages all read/write calls from the user script.
// Now it's pretty hacky and just has ReadN, but in the future it will have a
// rich set of read/write functions and be aware of the data type read/written
// (such as "these 8 bytes were read as an integer"), as well as functionalities
// to export this info to the UI.
class IOManager {
 public:
  explicit IOManager(Process* process);

  // Called by RPC thread. Does not block.
  // TODO: this should actually emit a response, the process could die for
  // example.
  void Write(const IOProducer& req);

  // Called by RPC thread. Does not block.
  void Read(const IOConsumer& req);

  // Called by RPC thread, blocks until data is obtained.
  void ReadSync(int chain_id, IOReadResult* result);

  // Called by UI-triggered RPC thread. Subscribes to UI update events.
  // The callback may be called with nullptr. It should return false if the
  // caller is no longer interested in events.
  void OnUIUpdate(const std::function<bool(const UIIODataUpdate&)>& callback);

 private:
  void MaybeSatisfyConsumers() EXCLUSIVE_LOCKS_REQUIRED(mutex_);

  struct PerConsumerData {
    absl::Time issuing_time;
    IOConsumer* original_request = nullptr;
    std::unique_ptr<ReadConsumer_Chain> chain;
    bool complete = false;
  };

  struct PerProducerData {
    absl::Time time;
    IOProducer* producer = nullptr;
  };

  struct RawOutputData {
    absl::Time time;
    std::string content;
    bool is_stderr;
  };

  struct IOUpdateState {
    std::function<bool(const UIIODataUpdate&)> callback;
    bool valid = true;
  };

  void SendIOUpdate(const UIIODataUpdate& update, IOUpdateState* state) {
    if (!state->valid) return;
    if (state->callback(update)) {
      state->valid = false;
    }
  }

  void SendIOUpdateForProducer(const PerProducerData& producer,
                               IOUpdateState* state) {
    UIIODataUpdate update;
    *update.mutable_input()->mutable_producer() = *producer.producer;
    update.set_timestamp(absl::ToUnixMicros(producer.time));
    SendIOUpdate(update, state);
  }

  void SendIOUpdateForConsumer(int chain_id, const PerConsumerData& consumer,
                               IOUpdateState* state) {
    UIIODataUpdate update;
    *update.mutable_structured_output()->mutable_req() =
        *consumer.original_request;
    update.mutable_structured_output()->set_chain_id(chain_id);
    update.mutable_structured_output()->set_complete(true);
    consumer.chain->Get(update.mutable_structured_output()->mutable_progress());
    update.set_timestamp(absl::ToUnixMicros(consumer.issuing_time));
    SendIOUpdate(update, state);
  }

  void SendIOUpdateForRawOutput(const RawOutputData& data,
                                IOUpdateState* state) {
    UIIODataUpdate update;
    update.set_timestamp(absl::ToUnixMicros(data.time));
    update.mutable_raw_output()->set_data(data.content);
    update.mutable_raw_output()->set_is_stderr(data.is_stderr);
    SendIOUpdate(update, state);
  }

  Process* const process_;
  std::string buf_ GUARDED_BY(mutex_);
  std::deque<int> pending_reads_ GUARDED_BY(mutex_);
  absl::flat_hash_map<int, PerConsumerData> chains_ GUARDED_BY(mutex_);
  std::vector<PerProducerData> all_producers_ GUARDED_BY(mutex_);
  std::vector<RawOutputData> raw_output_ GUARDED_BY(mutex_);
  std::vector<IOUpdateState> io_updates_ GUARDED_BY(mutex_);
  google::protobuf::Arena arena_;
  absl::Mutex mutex_;
};

#endif  // IO_MANAGER_H
