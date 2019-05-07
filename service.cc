#include "service.h"
#include "absl/strings/str_cat.h"
#include "lldb/API/SBDebugger.h"
#include "glog/logging.h"

grpc::Status UIServiceImpl::NewSession(::grpc::ServerContext* context,
                                       const ::NewSessionRequest* request,
                                       ::NewSessionResponse* response) {
  int session_id = next_session_++;
  LOG(INFO) << "Creating session " << session_id;
  SessionState::SessionStartOptions options;
  options.binary = request->binary();  // very primitive for now
  sessions_[session_id] = std::make_unique<SessionState>(options);
  auto session = sessions_[session_id].get();
  response->set_id(session_id);
  // do something with the session...
  return grpc::Status::OK;
}

grpc::Status UIServiceImpl::IOServerConnect(::grpc::ServerContext* context,
                                            ::grpc::ServerReaderWriter<::IOServerResponse,
                                                                       ::IOServerRequest>* stream) {
  IOServerRequest req;
  SessionState* session = nullptr;
  while (stream->Read(&req)) {
    LOG(INFO) << "IO driver -> " << req.DebugString();
    switch (req.request_case()) {
      case IOServerRequest::kStartId: {
        IOServerResponse resp;
        session = sessions_.at(req.start_id()).get();
        resp.set_ack(true);
        LOG(INFO) << "IO driver <- " << resp.DebugString();
        stream->Write(resp);
        break;
      }
      case IOServerRequest::kRead: {
        IOServerResponse resp;
        session->Read(req.read(), resp.mutable_read());
        LOG(INFO) << "IO driver <- " << resp.DebugString();
        stream->Write(resp);
        break;
      }
      case IOServerRequest::kWrite: {
        IOServerResponse resp;
        resp.set_ack(true);
        session->Write(req.write());
        LOG(INFO) << "IO driver <- " << resp.DebugString();
        stream->Write(resp);
        break;
      }
      case IOServerRequest::REQUEST_NOT_SET:break;
    }
  }
  return grpc::Status::OK;
}
