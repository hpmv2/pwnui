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
  // Reset the consumer's state.
  virtual void Reset() = 0;
  // Append data to the consumer's internal matching buffer.
  virtual void AppendAvailableData(std::string_view data) = 0;

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
  void Reset() override {
    buf_.clear();
    ptr_ = -1;
  }

  void AppendAvailableData(std::string_view data) override { buf_ += data; }

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
    result->set_data(buf_.substr(0, ptr_));
    result->mutable_any();
  }

 private:
  std::string buf_;
  int ptr_ = -1;
};

class ReadConsumer_Literal : public ReadConsumer {
 public:
  ReadConsumer_Literal(std::string expected) : expected_(std::move(expected)) {}
  void Reset() override {
    matched_.reset();
    buf_.clear();
  }

  void AppendAvailableData(std::string_view data) override { buf_ += data; }

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
    result->set_data(expected_);
  }

 private:
  std::string buf_;
  const std::string expected_;
  std::optional<bool> matched_;
};

class ReadConsumer_Number : public ReadConsumer {
 public:
  void Reset() override {
    buf_.clear();
    ptr_.reset();
  }

  void AppendAvailableData(std::string_view data) override { buf_ += data; }

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
    result->set_data(buf_.substr(0, *ptr_));
    result->mutable_number();
  }

 private:
  std::string buf_;
  std::optional<int> ptr_;
};

class ReadConsumer_Line : public ReadConsumer {
 public:
  explicit ReadConsumer_Line(std::unique_ptr<ReadConsumer> inner)
      : inner_(std::move(inner)) {}

  void Reset() override {
    buf_.clear();
    ptr_.reset();
    inner_->Reset();
  }

  void AppendAvailableData(std::string_view data) override { buf_ += data; }

  bool NextMatch(std::string_view* remaining) override {
    if (ptr_) return false;
    auto nl = buf_.find('\n');
    if (nl != std::string::npos) {
      inner_->AppendAvailableData(buf_.substr(0, nl));
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
    result->set_data(buf_.substr(0, *ptr_));
    inner_->Get(result->mutable_line()->mutable_inner());
  }

 private:
  std::string buf_;
  std::optional<int> ptr_;
  std::unique_ptr<ReadConsumer> inner_;
};

class ReadConsumer_Oneof : public ReadConsumer {
 public:
  ReadConsumer_Oneof(std::vector<std::unique_ptr<ReadConsumer>> alts)
      : alts_(std::move(alts)) {}

  void Reset() override {
    for (auto& alt : alts_) {
      alt->Reset();
    }
    alt_ = 0;
  }
  void AppendAvailableData(std::string_view data) override {
    for (auto& alt : alts_) {
      alt->AppendAvailableData(data);
    }
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
    Reset();
  }

  void Seal() { sealed_ = true; }
  bool sealed() const { return sealed_; }

  void Reset() override {
    for (auto& ele : elements_) {
      ele->Reset();
    }
  }

  void AppendAvailableData(std::string_view data) override {
    elements_[0]->AppendAvailableData(data);
  }

  bool MayHaveMoreMatches() override {
    for (auto& ele : elements_) {
      if (ele->MayHaveMoreMatches()) {
        return true;
      }
    }
    return false;
  }

  bool NextMatch(std::string_view* remaining) override {
    int i = valid_ ? elements_.size() - 1 : 0;
    while (true) {
      std::string_view rem;
      if (elements_[i]->NextMatch(&rem)) {
        i++;
        if (i == elements_.size()) {
          valid_ = true;
          *remaining = rem;
          return true;
        } else {
          elements_[i]->Reset();
          elements_[i]->AppendAvailableData(rem);
        }
      } else {
        i--;
        if (i == -1) {
          valid_ = false;
          return false;
        }
      }
    }
  }

  void Get(IOReadResult* result) override {
    for (auto& ele : elements_) {
      ele->Get(result->mutable_chain()->add_inner());
    }
    std::string data;
    for (const auto& inner : result->chain().inner()) {
      data += inner.data();
    }
    result->set_data(data);
  }

  bool valid() const { return valid_; }

 private:
  std::vector<std::unique_ptr<ReadConsumer>> elements_;
  bool sealed_ = false;
  bool valid_ = false;
  std::string buf_;
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
  void OnUIUpdate(const std::function<bool(const UIIODataUpdate*)>& callback);

 private:
  void MaybeSatisfyConsumers() EXCLUSIVE_LOCKS_REQUIRED(mutex_);
  UIIODataUpdate* NewUpdate() {
    return google::protobuf::Arena::CreateMessage<UIIODataUpdate>(&arena_);
  }

  Process* const process_;
  std::string buf_ GUARDED_BY(mutex_);
  std::deque<int> pending_reads_ GUARDED_BY(mutex_);
  absl::flat_hash_map<int, std::unique_ptr<ReadConsumer_Chain>> chains_
      GUARDED_BY(mutex_);
  google::protobuf::Arena arena_;
  absl::Mutex mutex_;
};

#endif  // IO_MANAGER_H
