#ifndef PROCESS_H
#define PROCESS_H

#include <boost/process.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/fiber/buffered_channel.hpp>

// Abstracts communication with an external process.
class Process {
 public:
  virtual ~Process() = default;

  // Begins constantly reading; the handler will be invoked whenever something is read.
  virtual void read(const std::function<void(std::string_view)>& handler) = 0;
  // Writes the given data; does not block.
  virtual void write(std::string_view data) = 0;
};

// A process running locally and directly, without any debugger.
class DirectExecutionProcess final : public Process {
 public:
  explicit DirectExecutionProcess(const std::string& binary);
  ~DirectExecutionProcess() override;
  void read(const std::function<void(std::string_view)>& handler) override;
  void write(std::string_view data) override;

 private:
  boost::asio::io_service ios_;
  boost::process::async_pipe stdin_pipe_{ios_};
  boost::process::async_pipe stdout_pipe_{ios_};
  std::unique_ptr<boost::process::child> child_;
  std::vector<char> buf_;
  std::unique_ptr<std::thread> reader_;
};

#endif  // PROCESS_H