#include <memory>

#include <memory>
#include <boost/process/io.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/read.hpp>

#include "process.h"
#include "glog/logging.h"

DirectExecutionProcess::~DirectExecutionProcess() {
  child_->terminate();
  child_->join();
  reader_->join();
}

DirectExecutionProcess::DirectExecutionProcess(const std::string& binary) : buf_(4096) {
  child_ = std::make_unique<boost::process::child>(binary, boost::process::std_in < stdin_pipe_,
                                                   boost::process::std_out
                                                       > stdout_pipe_, ios_);
}

void DirectExecutionProcess::read(const std::function<void(std::string_view)>& handler) {
  LOG(INFO) << "DirectExecutionProcess read called";
  reader_ = std::make_unique<std::thread>([this, handler] {
    LOG(INFO) << "DirectExecutionProcess read started";
    std::function<void()> reader = [&] {
      stdout_pipe_.async_read_some(boost::asio::buffer(buf_),
                                   [&](const
                                       boost::system::error_code& ec,
                                       std::size_t size) {
                                     LOG(INFO) << "DirectExecutionProcess stdout: "
                                               << std::string_view(buf_.data(), size);
                                     handler(std::string_view(buf_.data(), size));
                                     if (!ec) {
                                       reader();
                                     } else {
                                       stdout_pipe_.async_close();
                                     }
                                   });
    };
    reader();
    ios_.run();
  });
}

void DirectExecutionProcess::write(std::string_view data) {
  LOG(INFO) << "DirectExecutionProcess writing " << data;
  std::string* buf = new std::string(data);  // hacky, being lazy
  boost::asio::async_write(stdin_pipe_, boost::asio::buffer(*buf),
                           [=](const
                               boost::system::error_code& ec,
                               std::size_t transferred) {
                             LOG(INFO) << "DirectExecutionProcess wrote " << data;
                             delete buf;
                           });
}
