#ifndef SERVICE_H_
#define SERVICE_H_

#include "absl/container/flat_hash_map.h"
#include "service.grpc.pb.h"
#include "session-state.h"

class ServerState {
 public:
  int AddSession(SessionState::SessionStartOptions options) {
    sessions_[next_session_] =
        std::make_unique<SessionState>(next_session_, options);
    return next_session_++;
  }

  SessionState* GetSession(int id) { return sessions_.at(id).get(); }

 private:
  absl::flat_hash_map<int, std::unique_ptr<SessionState>> sessions_;
  int next_session_ = 1;
};

class UIServiceAsync {
 public:
  explicit UIServiceAsync(grpc::ServerCompletionQueue* cq,
                          UIService::AsyncService* service)
      : cq_(cq), service_(service) {}
  void Start();

 private:
  grpc::ServerCompletionQueue* const cq_;
  UIService::AsyncService* const service_;
  ServerState server_state_;
};

#endif  // SERVICE_H_