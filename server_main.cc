#include "service.h"
#include "grpcpp/grpcpp.h"
#include "gflags/gflags.h"
#include "absl/strings/str_cat.h"
#include "glog/logging.h"

DEFINE_int32(port, 12345, "Port number");

int main(int argc, char** argv) {
    FLAGS_logtostderr = true;
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    google::InitGoogleLogging(argv[0]);

    std::string address = absl::StrCat("0.0.0.0:", FLAGS_port);
    UIServiceImpl impl;
    grpc::ServerBuilder builder;
    builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    builder.RegisterService(&impl);
    auto server = builder.BuildAndStart();
    LOG(INFO) << "Server listening on " << address << "..." << std::endl;
    server->Wait();
}
