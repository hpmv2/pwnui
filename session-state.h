#ifndef SESSION_STATE_H
#define SESSION_STATE_H

#include <string>
#include <queue>
#include "process.h"
#include "io-manager.h"

// A very primitive implementation. Will change.
class SessionState {
 public:
  struct SessionStartOptions {
    std::string binary;
  };

  explicit SessionState(const SessionStartOptions& options) {
    // only support direct execution for now.
    process_ = std::make_unique<DirectExecutionProcess>(options.binary);
    io_manager_ = std::make_unique<IOManager>(process_.get());
  }

  void Read(const IOReadRequest& req, IOReadResponse* resp) {
    return io_manager_->Read(req, resp);
  }

  void Write(const IOWriteRequest& req){
    io_manager_->Write(req);
  }

 private:
  std::unique_ptr<IOManager> io_manager_;  // must be placed before Process for destruction order.
  std::unique_ptr<Process> process_;
};

#endif  // SESSION_STATE_H
