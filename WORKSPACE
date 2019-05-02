workspace(name = "pwn")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

# Set up LLVM dependencies
new_local_repository(
    name = "llvm_headers",
    build_file = "llvm_headers.BUILD",
    path = "/usr/lib/llvm-8/include",
)

new_local_repository(
    name = "llvm_libs",
    build_file = "llvm_libs.BUILD",
    path = "/usr/lib/llvm-8/lib",
)

# Set up gRPC
git_repository(
    name = "com_github_grpc_grpc",
    commit = "7741e806a213cba63c96234f16d712a8aa101a49",
    remote = "https://github.com/grpc/grpc.git",
    shallow_since = "1556224604 -0700",
)

load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps")

grpc_deps()

# Set up glog + gflags
git_repository(
    name = "com_github_google_glog",
    commit = "0d0c254317062f79c7e704d7169d56b13adc013a",
    remote = "https://github.com/google/glog.git",
    shallow_since = "1556070531 +0900",
)

# Set up gRPC Web
git_repository(
    name = "com_github_grpc_grpc_web",
    commit = "9593703052797dd40915201c2533cf3b862970dd",
    remote = "https://github.com/grpc/grpc-web.git",
)

git_repository(
    name = "io_bazel_rules_closure",
    commit = "a176ec89a1b251bb5442ba569d47cee3c053e633",
    remote = "https://github.com/bazelbuild/rules_closure.git",
)

load("@io_bazel_rules_closure//closure:defs.bzl", "closure_repositories")

closure_repositories()

git_repository(
    name = "com_google_protobuf",
    commit = "v3.7.0",
    remote = "https://github.com/google/protobuf.git",
)
