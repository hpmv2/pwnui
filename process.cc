#include <memory>

#include <boost/asio/buffer.hpp>
#include <boost/asio/read.hpp>
#include <boost/process/args.hpp>
#include <boost/process/io.hpp>
#include <memory>

#include "glog/logging.h"
#include "process.h"

DirectExecutionProcess::~DirectExecutionProcess() {
  child_->terminate();
  child_->join();
  reader_->join();
}

DirectExecutionProcess::DirectExecutionProcess(
    const std::string& binary, const std::vector<std::string>& args)
    : buf_(4096) {
  try {
    child_ = std::make_unique<boost::process::child>(
        binary, boost::process::args(args),
        (boost::process::std_in < stdin_pipe_),
        (boost::process::std_out > stdout_pipe_),
        (boost::process::std_err > stderr_pipe_), ios_);
  } catch (...) {
    abort();
  }
}

void DirectExecutionProcess::read(
    const std::function<void(std::string_view, bool)>& handler) {
  LOG(INFO) << "DirectExecutionProcess read called";
  reader_ = std::make_unique<std::thread>([this, handler] {
    LOG(INFO) << "DirectExecutionProcess read started";
    std::function<void()> reader = [&] {
      stdout_pipe_.async_read_some(
          boost::asio::buffer(buf_),
          [&](const boost::system::error_code& ec, std::size_t size) {
            LOG(INFO) << "DirectExecutionProcess stdout: "
                      << std::string_view(buf_.data(), size);
            handler(std::string_view(buf_.data(), size), false);
            if (!ec && size > 0) {
              reader();
            } else {
              stdout_pipe_.async_close();
            }
          });
    };
    reader();

    std::function<void()> reader_err = [&] {
      stderr_pipe_.async_read_some(
          boost::asio::buffer(buf_err_),
          [&](const boost::system::error_code& ec, std::size_t size) {
            LOG(INFO) << "DirectExecutionProcess stderr: "
                      << std::string_view(buf_err_.data(), size);
            handler(std::string_view(buf_err_.data(), size), true);
            if (!ec && size > 0) {
              reader_err();
            } else {
              stderr_pipe_.async_close();
            }
          });
    };
    reader_err();
    ios_.run();
  });
}

void DirectExecutionProcess::write(std::string_view data) {
  LOG(INFO) << "DirectExecutionProcess writing " << data;
  std::string* buf = new std::string(data);  // hacky, being lazy
  boost::asio::async_write(
      stdin_pipe_, boost::asio::buffer(*buf),
      [=](const boost::system::error_code& ec, std::size_t transferred) {
        LOG(INFO) << "DirectExecutionProcess wrote " << data;
        delete buf;
      });
}
