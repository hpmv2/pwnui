#ifndef IO_MANAGER_H
#define IO_MANAGER_H

#include "process.h"
#include "absl/synchronization/mutex.h"

// Manages all read/write calls from the user script.
// Now it's pretty hacky and just has ReadN, but in the future it will have a rich set of read/write
// functions and be aware of the data type read/written (such as "these 8 bytes were read as an
// integer"), as well as functionalities to export this info to the UI.
class IOManager {
 public:
  explicit IOManager(Process* process);

  // Called by RPC thread, blocks until bytes available.
  std::string ReadN(int bytes);

  // Called by RPC thread. Does not block.
  void Write(std::string_view data) {
    process_->write(data);
  }

 private:
  Process* process_;
  std::deque<char> buffer_;
  absl::Mutex mutex_;
};

#endif  // IO_MANAGER_H
