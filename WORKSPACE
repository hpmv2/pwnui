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

# Set up googletest
git_repository(
    name = "com_github_google_googletest",
    commit = "e110929a7b496714c1f6f6be2edcf494a18e5676",
    shallow_since = "1560526595 -0400",
    remote = "https://github.com/google/googletest",
)

# Set up gRPC
git_repository(
    name = "com_github_grpc_grpc",
    commit = "1ef80f46bd9176b8411196b864fd174a2c007bf2",
    remote = "https://github.com/grpc/grpc.git",
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
    commit = "6973c3a5041636c1d8dc5f7f6c8c1f3c15bc63d6",
    remote = "https://github.com/google/protobuf.git",
)

# A bunch of python stuff
git_repository(
    name = "build_stack_rules_proto",
    commit = "ca73549e5e7490ee32a87ad2cfecbc062c82c249",
    remote = "https://github.com/stackb/rules_proto.git",
)

load("@build_stack_rules_proto//python:deps.bzl", "python_grpc_library")

python_grpc_library()

load("@io_bazel_rules_python//python:pip.bzl", "pip_import", "pip_repositories")

pip_repositories()

pip_import(
    name = "protobuf_py_deps",
    requirements = "@build_stack_rules_proto//python/requirements:protobuf.txt",
)

load("@protobuf_py_deps//:requirements.bzl", protobuf_pip_install = "pip_install")

protobuf_pip_install()

pip_import(
    name = "grpc_py_deps",
    requirements = "@build_stack_rules_proto//python:requirements.txt",
)

load("@grpc_py_deps//:requirements.bzl", grpc_pip_install = "pip_install")

grpc_pip_install()

# Boost stuff
git_repository(
    name = "com_github_nelhage_rules_boost",
    commit = "6d6fd834281cb8f8e758dd9ad76df86304bf1869",
    remote = "https://github.com/nelhage/rules_boost",
)

load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")

boost_deps()
