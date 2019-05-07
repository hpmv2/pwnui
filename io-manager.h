#ifndef IO_MANAGER_H
#define IO_MANAGER_H

#include <queue>
#include <regex>
#include "process.h"
#include "absl/synchronization/mutex.h"
#include "absl/container/flat_hash_map.h"
#include "google/protobuf/arena.h"
#include "service.pb.h"

class ElementReadState {
 public:
  virtual ~ElementReadState() = default;
  // Processes data as it comes in (or is already available); if any data is consumed, mutates
  // the data string_view to become a suffix of the original, and adds elements to the array to
  // reflect the current state. When the reading is done, returns an IOReadResultElement.
  virtual IOReadResultElement* ProcessAvailableData(std::string_view* data,
                                                    std::vector<IOElement*>* elements,
                                                    google::protobuf::Arena* arena) = 0;
};

// Manages all read/write calls from the user script.
// Now it's pretty hacky and just has ReadN, but in the future it will have a rich set of read/write
// functions and be aware of the data type read/written (such as "these 8 bytes were read as an
// integer"), as well as functionalities to export this info to the UI.
class IOManager {
 public:
  explicit IOManager(Process* process);

  // Called by RPC thread. Does not block.
  void Write(const IOWriteRequest& req);

  // Called by RPC thread, blocks until the read is complete.
  void Read(const IOReadRequest& req, IOReadResponse* resp);

 private:
  void ProcessDataForFrontReader(std::string_view* data);

  Process* process_;
  std::deque<std::string> buffer_;
  std::queue<std::unique_ptr<ElementReadState>> pending_reads_;
  std::queue<IOReadResultElement*> read_results_;
  google::protobuf::Arena arena_;
  std::vector<IOChunk> all_chunks;
  std::vector<std::pair<bool, int>> chunk_history;  // true = add, false = remove
  absl::Mutex mutex_;
};

class ElementReadState_ReadLittleEndian32 : public ElementReadState {
 public:
  IOReadResultElement* ProcessAvailableData(std::string_view* data,
                                            std::vector<IOElement*>* elements,
                                            google::protobuf::Arena* arena) override;

 private:
  std::string current_;
};

class ElementReadState_ReadRegex : public ElementReadState {
 public:
  ElementReadState_ReadRegex(const std::string& regex, std::vector<IOElementType> groups) :
      re_(regex), groups_(std::move(groups)) {}
  IOReadResultElement* ProcessAvailableData(std::string_view* data,
                                            std::vector<IOElement*>* elements,
                                            google::protobuf::Arena* arena) override;
 private:
  const std::regex re_;
  const std::vector<IOElementType> groups_;
  std::string current_;
};

#endif  // IO_MANAGER_H
