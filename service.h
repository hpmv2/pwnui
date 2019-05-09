#ifndef SERVICE_H_
#define SERVICE_H_

#include "absl/container/flat_hash_map.h"
#include "service.grpc.pb.h"
#include "session-state.h"

class UIServiceImpl : public UIService::Service {
 public:
  grpc::Status NewSession(::grpc::ServerContext* context,
                          const ::NewSessionRequest* request,
                          ::NewSessionResponse* response) override;

  grpc::Status IOServerConnect(
      ::grpc::ServerContext* context,
      ::grpc::ServerReaderWriter<::IOServerResponse, ::IOServerRequest>* stream)
      override;

  grpc::Status GetIODataForUI(
      ::grpc::ServerContext* context, const UIIODataRequest* request,
      ::grpc::ServerWriter<UIIODataUpdate>* writer) override;

 private:
  absl::flat_hash_map<int, std::unique_ptr<SessionState>> sessions_;
  int next_session_ = 1;
};

#endif  // SERVICE_H_