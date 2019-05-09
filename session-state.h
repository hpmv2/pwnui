#ifndef SESSION_STATE_H
#define SESSION_STATE_H

#include <queue>
#include <string>
#include "io-manager.h"
#include "process.h"

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

  IOManager* io_manager() { return io_manager_.get(); }

 private:
  std::unique_ptr<IOManager>
      io_manager_;  // must be placed before Process for destruction order.
  std::unique_ptr<Process> process_;
};

#endif  // SESSION_STATE_H
