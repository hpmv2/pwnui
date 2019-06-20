#include "absl/debugging/failure_signal_handler.h"
#include "absl/debugging/symbolize.h"
#include "absl/strings/str_cat.h"
#include "boost/filesystem.hpp"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include "grpcpp/grpcpp.h"
#include "service.h"

DEFINE_int32(port, 12345, "Port number");

void IgnoreSigpipe() {
  struct sigaction sa;
  std::memset(&sa, 0, sizeof(sa));
  sa.sa_handler = SIG_IGN;
  int res = sigaction(SIGPIPE, &sa, NULL);
  assert(res == 0);
}

int main(int argc, char** argv) {
  FLAGS_logtostderr = true;
  absl::InitializeSymbolizer(argv[0]);
  absl::FailureSignalHandlerOptions options;
  absl::InstallFailureSignalHandler(options);
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);

  IgnoreSigpipe();

  if (!boost::filesystem::exists("io_driver/driver")) {
    LOG(FATAL) << "pwd not correct. Run with bazel run.";
  }

  std::string address = absl::StrCat("0.0.0.0:", FLAGS_port);
  UIService::AsyncService service;
  grpc::ServerBuilder builder;
  builder.AddListeningPort(address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  auto cq = builder.AddCompletionQueue();
  auto server = builder.BuildAndStart();
  LOG(INFO) << "Server listening on " << address << "..." << std::endl;
  UIServiceAsync async(cq.get(), &service);
  async.Start();
}
