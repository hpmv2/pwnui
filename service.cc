#include "service.h"
#include "absl/strings/str_cat.h"
#include "glog/logging.h"
#include "lldb/API/SBDebugger.h"

namespace {

template <class T>
void* TagPtr(T* t, int tag) {
  CHECK_LE(tag, alignof(T));
  return reinterpret_cast<void*>(reinterpret_cast<intptr_t>(t) | tag);
}

template <class T>
T* GetPtrPart(void* ptr) {
  return reinterpret_cast<T*>(reinterpret_cast<intptr_t>(ptr) &
                              ~static_cast<intptr_t>(alignof(T) - 1));
}

template <class T>
int GetTagPart(void* ptr) {
  return reinterpret_cast<intptr_t>(ptr) &
         static_cast<intptr_t>(alignof(T) - 1);
}

class AsyncCallHandler {
 public:
  AsyncCallHandler(UIService::AsyncService* service,
                   grpc::ServerCompletionQueue* cq, ServerState* server_state)
      : service_(service), cq_(cq), server_state_(server_state) {}
  virtual ~AsyncCallHandler() = default;
  virtual void Proceed(int aux) = 0;
  virtual std::string Describe() = 0;

 protected:
  void* tag(int aux = 0) { return TagPtr(this, aux); }

  UIService::AsyncService* const service_;
  grpc::ServerCompletionQueue* const cq_;
  ServerState* const server_state_;
  grpc::ServerContext ctx_;
};

class NewSessionHandler : public AsyncCallHandler {
 public:
  using AsyncCallHandler::AsyncCallHandler;

  void Proceed(int aux) override {
    if (state_ == INITIAL) {
      service_->RequestNewSession(&ctx_, &req_, &resp_, cq_, cq_, tag());
      state_ = HANDLE;
    } else if (state_ == HANDLE) {
      (new NewSessionHandler(service_, cq_, server_state_))->Proceed(0);

      SessionState::SessionStartOptions options;
      options.binary = req_.binary();  // very primitive for now
      options.script_content = req_.script();
      auto session_id = server_state_->AddSession(options);
      LOG(INFO) << "Created session " << session_id << " with request "
                << req_.DebugString();
      NewSessionResponse resp;
      resp.set_id(session_id);
      resp_.Finish(resp, grpc::Status::OK, tag());
      state_ = FINISH;
    } else {
      CHECK_EQ(state_, FINISH);
      delete this;
    }
  }

  std::string Describe() override {
    return absl::StrCat("NewSessionHandler[state ", state_, "]");
  }

 private:
  enum { INITIAL, HANDLE, FINISH } state_ = INITIAL;
  NewSessionRequest req_;
  grpc::ServerAsyncResponseWriter<NewSessionResponse> resp_{&ctx_};
};

class IOServerHandler : public AsyncCallHandler {
 public:
  using AsyncCallHandler::AsyncCallHandler;

  void Proceed(int aux) override {
    if (aux == TAG_DONE) {
      LOG(INFO) << "RPC is done";
      delete this;
    }
    if (state_ == INITIAL) {
      ctx_.AsyncNotifyWhenDone(tag(TAG_DONE));
      service_->RequestIOServerConnect(&ctx_, &io_, cq_, cq_, tag());
      state_ = HANDSHAKE;
    } else if (state_ == HANDSHAKE) {
      (new IOServerHandler(service_, cq_, server_state_))->Proceed(0);
      state_ = READ;
      Proceed(0);
    } else if (state_ == READ) {
      io_.Read(&req_, tag());
      state_ = WRITE;
    } else if (state_ == WRITE) {
      switch (req_.request_case()) {
        case IOServerRequest::kStartId: {
          IOServerResponse resp;
          LOG(INFO) << "IO driver <- " << req_.ShortDebugString();
          session_ = server_state_->GetSession(req_.start_id());
          resp.set_ack(true);
          io_.Write(resp, tag());
          break;
        }
        case IOServerRequest::kRead: {
          IOServerResponse resp;
          resp.set_ack(true);
          LOG(INFO) << "IO driver <- " << req_.ShortDebugString();
          session_->io_manager()->Read(req_.read());
          io_.Write(resp, tag());
          break;
        }
        case IOServerRequest::kWrite: {
          IOServerResponse resp;
          resp.set_ack(true);
          LOG(INFO) << "IO driver <- " << req_.ShortDebugString();
          session_->io_manager()->Write(req_.write());
          io_.Write(resp, tag());
          break;
        }
        case IOServerRequest::kSync: {
          IOServerResponse resp;
          // TODO: This is blocking, which is problematic. Make it not block.
          LOG(INFO) << "IO driver <- " << req_.ShortDebugString();
          session_->io_manager()->ReadSync(req_.sync().chain_id(),
                                           resp.mutable_result());
          LOG(INFO) << "IO driver -> " << resp.ShortDebugString();
          io_.Write(resp, tag());
          break;
        }
        case IOServerRequest::REQUEST_NOT_SET:
          break;
      }
      state_ = READ;
    }
  }

  std::string Describe() override {
    return absl::StrCat("IOServerHandler[state ", state_, "] ",
                        session_ ? session_->session_id() : -1);
  }

 private:
  enum { TAG_DONE = 1 };
  enum { INITIAL, HANDSHAKE, READ, WRITE } state_ = INITIAL;
  grpc::ServerAsyncReaderWriter<IOServerResponse, IOServerRequest> io_{&ctx_};
  IOServerRequest req_;
  SessionState* session_ = nullptr;
};

class GetIODataForUIHandler : public AsyncCallHandler {
 public:
  using AsyncCallHandler::AsyncCallHandler;

  void Proceed(int aux) override {
    if (aux == TAG_DONE) {
      LOG(INFO) << "RPC is done";
      delete this;
    }
    if (state_ == INITIAL) {
      LOG(INFO) << "Setting up";
      ctx_.AsyncNotifyWhenDone(tag(TAG_DONE));
      service_->RequestGetIODataForUI(&ctx_, &req_, &writer_, cq_, cq_, tag());
      state_ = HANDSHAKE;
    } else if (state_ == HANDSHAKE) {
      (new GetIODataForUIHandler(service_, cq_, server_state_))->Proceed(0);
      auto* session_state = server_state_->GetSession(req_.session_id());
      session_state->OnDriverOutput([this](const UIIODriverOutput& output) {
        UIIODataUpdate update;
        *update.mutable_driver_output() = output;
        absl::MutexLock l(&mutex_);
        pending_writes_.push(std::move(update));
        if (!is_waiting_for_write_) {
          ProcessQueue();
        }
        // TODO: Use some better mechanism to cancel the subscription.
      });
      session_state->io_manager()->OnUIUpdate(
          [this](const UIIODataUpdate& update) {
            absl::MutexLock l(&mutex_);
            pending_writes_.push(update);
            if (!is_waiting_for_write_) {
              ProcessQueue();
            }
            // TODO: Use some better mechanism to cancel the subscription.
            return false;
          });

      state_ = UPDATE;
    } else if (state_ == UPDATE) {
      absl::MutexLock l(&mutex_);
      is_waiting_for_write_ = false;
      ProcessQueue();
    }
  }

  std::string Describe() override {
    return absl::StrCat("GetIODataForUIHandler[state ", state_, "] ",
                        req_.session_id());
  }

 private:
  void ProcessQueue() EXCLUSIVE_LOCKS_REQUIRED(mutex_) {
    if (!pending_writes_.empty()) {
      is_waiting_for_write_ = true;
      LOG(INFO) << "UI IO Update -> " << pending_writes_.front().DebugString();
      writer_.Write(pending_writes_.front(), tag());
      pending_writes_.pop();
    }
  }

  enum { TAG_DONE = 1 };
  enum { INITIAL, HANDSHAKE, UPDATE, FINISH } state_ = INITIAL;

  UIIODataRequest req_;
  grpc::ServerAsyncWriter<UIIODataUpdate> writer_{&ctx_};

  std::queue<UIIODataUpdate> pending_writes_ GUARDED_BY(mutex_);
  bool is_waiting_for_write_ GUARDED_BY(mutex_) = false;
  absl::Mutex mutex_;
};

}  // namespace

void UIServiceAsync::Start() {
  (new NewSessionHandler(service_, cq_, &server_state_))->Proceed(0);
  (new IOServerHandler(service_, cq_, &server_state_))->Proceed(0);
  (new GetIODataForUIHandler(service_, cq_, &server_state_))->Proceed(0);
  void* tag = nullptr;
  bool ok = false;
  while (cq_->Next(&tag, &ok)) {
    if (!ok) {
      LOG(WARNING) << "CompletionQueue failed for "
                   << GetPtrPart<AsyncCallHandler>(tag)->Describe();
      delete GetPtrPart<AsyncCallHandler>(tag);
    } else {
      GetPtrPart<AsyncCallHandler>(tag)->Proceed(
          GetTagPart<AsyncCallHandler>(tag));
    }
  }
}
