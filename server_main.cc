#include "absl/debugging/failure_signal_handler.h"
#include "absl/debugging/symbolize.h"
#include "absl/strings/str_cat.h"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include "grpcpp/grpcpp.h"
#include "service.h"

DEFINE_int32(port, 12345, "Port number");

void IgnoreSigpipe() {
  struct sigaction sa;
  std::memset( &sa, 0, sizeof(sa) );
  sa.sa_handler = SIG_IGN;
  int res = sigaction( SIGPIPE, &sa, NULL);
  assert( res == 0 );
}

int main(int argc, char **argv) {
  FLAGS_logtostderr = true;
  absl::InitializeSymbolizer(argv[0]);
  absl::FailureSignalHandlerOptions options;
  absl::InstallFailureSignalHandler(options);
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);

  IgnoreSigpipe();

  std::string address = absl::StrCat("0.0.0.0:", FLAGS_port);
  UIServiceImpl impl;
  grpc::ServerBuilder builder;
  builder.AddListeningPort(address, grpc::InsecureServerCredentials());
  builder.RegisterService(&impl);
  auto server = builder.BuildAndStart();
  LOG(INFO) << "Server listening on " << address << "..." << std::endl;
  server->Wait();
}
