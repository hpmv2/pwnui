## Demo

IO DSL:

![IO DSL](demo/io-dsl.png)

IO visualization UI:

![IO Demo](demo/io-demo.gif)

(Old) IO script and visualization

![Integrated editor](demo/integrated-editor.png)

## Setup

Install LLVM toolchains and dev headers

* Add source `http://apt.llvm.org/bionic llvm-toolchain-bionic-8`
* `sudo apt install clang-8 libllvm8 liblldb-8-dev`

Install Bazel and protoc

* https://docs.bazel.build/versions/master/install-ubuntu.html
* protoc MUST NOT be installed from APT. Its version is way too archaic. Download from [here](https://gist.github.com/ryujaehun/991f5f1e8c1485dea72646877707f497) and just put protoc in PATH.

Install Docker

* Follow the instructions on https://docs.docker.com/install/linux/docker-ce/ubuntu/

Development shortcuts:

* Follow instructions here to set up clangd autocompletion for vscode/vim: https://github.com/grailbio/bazel-compilation-database
* Use the following flag in your `~/.blazerc` to speed up compilation, especially if you swap between -c dbg and -c opt, etc.
  * `build --disk_cache=/tmp/bazel_disk_cache`

## Build

`bazel build :server_main`

## Run

Server:

`bazel run :server_main -- --port=12345`

Proxy:

```
cd frontend
sudo docker build -t helloworld/envoy -f ./envoy.Dockerfile .
docker run -d -p 12346:12346 --network=host helloworld/envoy
```

Client:

```
cd frontend/pwnui
ng serve --aot
```

## Develop

Be sure to rerun `frontend/update_frontend_grpc_genfiles.sh` whenever updating `service.proto`. The frontend codegen is not automated.
