#include "io-manager.h"

IOManager::IOManager(Process* process) : process_(process) {
  process_->read([this](std::string_view bytes) {
    absl::MutexLock l(&mutex_);
    std::copy(bytes.begin(), bytes.end(), std::back_inserter(buffer_));
  });
}

std::string IOManager::ReadN(int bytes) {
  struct Args {
    IOManager* me;
    int bytes;
  } args{this, bytes};

  absl::Condition condition(+[](Args* args) {
    return args->me->buffer_.size() >= args->bytes;
  }, &args);
  mutex_.LockWhen(condition);
  std::string s(&*buffer_.begin(), static_cast<size_t>(bytes));
  buffer_.erase(buffer_.begin(), buffer_.begin() + bytes);
  mutex_.Unlock();
  return s;
}