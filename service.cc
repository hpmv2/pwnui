#include "service.h"
#include "absl/strings/str_cat.h"
#include "lldb/API/SBDebugger.h"
#include "glog/logging.h"

grpc::Status UIServiceImpl::NewSession(::grpc::ServerContext *context, const ::NewSessionRequest *request,
                                       ::NewSessionResponse *response) {
    response->set_id(1234);
    if (2 + 2 == 5) {
        // Just to test compilation linking at this point.
        lldb::SBDebugger::Initialize();
        LOG(INFO) << absl::StrCat("abc", 8, "def");
    }
    return grpc::Status::OK;
}
