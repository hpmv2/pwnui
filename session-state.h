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
    std::string script_content;
  };

  explicit SessionState(int session_id, const SessionStartOptions& options)
      : session_id_(session_id) {
    // only support direct execution for now.
    process_ = std::make_unique<DirectExecutionProcess>(
        options.binary, std::vector<std::string>());
    io_manager_ = std::make_unique<IOManager>(process_.get());
    RunIOScript(options.script_content);
  }

  IOManager* io_manager() { return io_manager_.get(); }
  int session_id() const { return session_id_; }

  void OnDriverOutput(
      const std::function<void(const UIIODriverOutput& output)>& callback);

 private:
  // Begins running an IO script which will call back to this server to make
  // I/O requests. Destroying the SessionState will kill the script.
  void RunIOScript(const std::string& script);

  // This must be placed before Process for destruction order.
  const int session_id_;
  std::unique_ptr<IOManager> io_manager_;
  std::unique_ptr<Process> process_;
  std::unique_ptr<Process> io_driver_;
};

#endif  // SESSION_STATE_H
