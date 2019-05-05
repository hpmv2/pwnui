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
    switch (req.request_case()) {
      case IOServerRequest::kStartId: {
        LOG(INFO) << "IO server ready for session " << req.start_id();
        IOServerResponse resp;
        session = sessions_.at(req.start_id()).get();
        resp.set_ack(true);
        stream->Write(resp);
        break;
      }
      case IOServerRequest::kReadn: {
        LOG(INFO) << "IO server reading " << req.readn() << " bytes";
        IOServerResponse resp;
        resp.set_content(session->ReadN(req.readn()));
        stream->Write(resp);
        break;
      }
      case IOServerRequest::kWrite: {
        LOG(INFO) << "IO server writing " << req.write();
        IOServerResponse resp;
        resp.set_ack(true);
        session->Write(req.write());
        stream->Write(resp);
        break;
      }
      case IOServerRequest::REQUEST_NOT_SET:break;
    }
  }
  return grpc::Status::OK;
}
