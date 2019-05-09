#ifndef IO_MANAGER_H
#define IO_MANAGER_H

#include <queue>
#include <regex>
#include "absl/container/flat_hash_map.h"
#include "absl/synchronization/mutex.h"
#include "google/protobuf/arena.h"
#include "process.h"
#include "service.pb.h"

class IOElementAdder {
 public:
  explicit IOElementAdder(
      google::protobuf::RepeatedPtrField<UIIOElement>* field)
      : field_(field) {}
  void Add(const std::string& data, IOElementType type) {
    auto element = field_->Add();
    element->set_data(data);
    element->set_type(type);
  }
  void AddPending(const std::string& data, IOElementType type) {
    auto element = field_->Add();
    element->set_data(data);
    element->set_type(type);
    element->set_pending(true);
  }

 private:
  google::protobuf::RepeatedPtrField<UIIOElement>* field_;
};

class ChunkReadState {
 public:
  ChunkReadState(int chunk_id) : chunk_id_(chunk_id) {}

  virtual ~ChunkReadState() = default;
  // Processes data as it comes in (or is already available); if any data is
  // consumed, mutates the data string_view to become a suffix of the original,
  // and adds elements to the array to reflect the current state. When the
  // reading is done, returns an IOReadResultElement.
  virtual IOReadChunkResult* ProcessAvailableData(
      std::string_view* data, IOElementAdder* adder,
      google::protobuf::Arena* arena) = 0;

  int chunk_id() const { return chunk_id_; }

 private:
  int chunk_id_ = -1;
};

struct PendingDataState {
  int chunk_id = -1;
  std::string data;
};

// Manages all read/write calls from the user script.
// Now it's pretty hacky and just has ReadN, but in the future it will have a
// rich set of read/write functions and be aware of the data type read/written
// (such as "these 8 bytes were read as an integer"), as well as functionalities
// to export this info to the UI.
class IOManager {
 public:
  explicit IOManager(Process* process);

  // Called by RPC thread. Does not block.
  // TODO: this should actually emit a response, the process could die for
  // example.
  void Write(const IOWriteRequest& req);

  // Called by RPC thread, blocks until the read is complete.
  void Read(const IOReadRequest& req, IOReadResponse* resp);

  // Called by UI-triggered RPC thread. Subscribes to UI update events.
  // The callback may be called with nullptr. It should return false if the
  // caller is no longer interested in events.
  void OnUIUpdate(const std::function<bool(const UIIODataUpdate*)>& callback);

 private:
  void ProcessDataForFrontReader(std::string_view* data);
  void AddUIUpdate(const UIIODataUpdate* update);
  UIIODataUpdate* NewUpdate() {
    return google::protobuf::Arena::CreateMessage<UIIODataUpdate>(&arena_);
  }

  Process* process_;
  std::deque<PendingDataState> buffer_;
  std::queue<std::unique_ptr<ChunkReadState>> pending_reads_;
  std::queue<IOReadChunkResult*> read_results_;
  int next_chunk_id_ = 1;
  google::protobuf::Arena arena_;
  absl::Mutex mutex_;

  std::vector<const UIIODataUpdate*> all_updates_;
  absl::Mutex updates_mutex_;
};

class ChunkReadState_ReadLittleEndian32 : public ChunkReadState {
 public:
  explicit ChunkReadState_ReadLittleEndian32(int chunk_id)
      : ChunkReadState(chunk_id) {}
  IOReadChunkResult* ProcessAvailableData(
      std::string_view* data, IOElementAdder* adder,
      google::protobuf::Arena* arena) override;

 private:
  std::string current_;
};

class ChunkReadState_ReadRegex : public ChunkReadState {
 public:
  ChunkReadState_ReadRegex(int chunk_id, const std::string& regex,
                           const std::vector<IOElementType>& groups)
      : ChunkReadState(chunk_id), re_(regex), groups_(groups) {}
  IOReadChunkResult* ProcessAvailableData(
      std::string_view* data, IOElementAdder* adder,
      google::protobuf::Arena* arena) override;

 private:
  const std::regex re_;
  const std::vector<IOElementType> groups_;
  std::string current_;
};

#endif  // IO_MANAGER_H
