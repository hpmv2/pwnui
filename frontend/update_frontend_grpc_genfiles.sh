#!/bin/bash

# Run this every time you update service.proto.
# The build system could probably do this instead, but:
#  1. I don't know how to do that with the angular build system
#  2. Even if I could, I don't know how to make IDEs like vscode discover the generated files to provide autocompletion.
#
# So instead we'll just check in the generated files as source.

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

cd $DIR
bazel build :frontend_grpc_gen
cp ../bazel-genfiles/frontend/service_pb.js pwnui/src/rpc
cp ../bazel-genfiles/frontend/service_pb.d.ts pwnui/src/rpc
cp ../bazel-genfiles/frontend/ServiceServiceClientPb.ts pwnui/src/rpc
chmod 664 pwnui/src/rpc/service_pb.js
chmod 664 pwnui/src/rpc/service_pb.d.ts
chmod 664 pwnui/src/rpc/ServiceServiceClientPb.ts
