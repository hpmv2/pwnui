#include <boost/asio/buffer.hpp>
#include <boost/asio/read.hpp>
#include <boost/process/args.hpp>
#include <boost/process/io.hpp>
#include <memory>

#include "glog/logging.h"
#include "process.h"

DirectExecutionProcess::~DirectExecutionProcess() {
  LOG(INFO) << binary_ << " terminating.";
  child_->terminate();
  child_->join();
  reader_->join();
}

DirectExecutionProcess::DirectExecutionProcess(
    const std::string& binary, const std::vector<std::string>& args)
    : binary_(binary), buf_(4096), buf_err_(4096) {
  LOG(INFO) << binary_ << " starting.";
  try {
    child_ = std::make_unique<boost::process::child>(
        binary, boost::process::args(args),
        (boost::process::std_in < stdin_pipe_),
        (boost::process::std_out > stdout_pipe_),
        (boost::process::std_err > stderr_pipe_), ios_);
  } catch (const boost::process::process_error& error) {
    LOG(FATAL) << error.code() << ": " << error.what();
  }
}

void DirectExecutionProcess::read(
    const std::function<void(std::string_view, bool)>& handler) {
  reader_ = std::make_unique<std::thread>([this, handler] {
    std::function<void()> reader = [&] {
      stdout_pipe_.async_read_some(
          boost::asio::buffer(buf_),
          [&](const boost::system::error_code& ec, std::size_t size) {
            if (!ec && size > 0) {
              LOG(INFO) << binary_ << "[stdout] "
                        << std::string_view(buf_.data(), size);
              handler(std::string_view(buf_.data(), size), false);
              reader();
            } else {
              LOG(INFO) << binary_ << "[stdout closed]";
              stdout_pipe_.async_close();
            }
          });
    };
    reader();

    std::function<void()> reader_err = [&] {
      stderr_pipe_.async_read_some(
          boost::asio::buffer(buf_err_),
          [&](const boost::system::error_code& ec, std::size_t size) {
            if (!ec && size > 0) {
              LOG(INFO) << binary_ << "[stderr] "
                        << std::string_view(buf_err_.data(), size);
              handler(std::string_view(buf_err_.data(), size), true);
              reader_err();
            } else {
              LOG(INFO) << binary_ << "[stderr closed]";
              stderr_pipe_.async_close();
            }
          });
    };
    reader_err();
    ios_.run();
  });
}

void DirectExecutionProcess::write(std::string_view data) {
  LOG(INFO) << binary_ << "[stdin] " << data;
  std::string* buf = new std::string(data);  // hacky, being lazy
  boost::asio::async_write(stdin_pipe_, boost::asio::buffer(*buf),
                           [=](const boost::system::error_code& ec,
                               std::size_t transferred) { delete buf; });
}
