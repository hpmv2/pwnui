#include <boost/filesystem.hpp>

#include "glog/logging.h"
#include "session-state.h"

void SessionState::RunIOScript(const std::string& script) {
  auto path = boost::filesystem::temp_directory_path();
  boost::filesystem::create_directory(path);
  auto script_file_path = absl::StrCat(path.string(), "/io_script.py");
  std::ofstream script_file(script_file_path);
  script_file << script;
  script_file.close();
  io_driver_ = std::make_unique<DirectExecutionProcess>(
      "io_driver/driver",
      std::vector<std::string>{absl::StrCat(session_id_), script_file_path});
  io_driver_->read([](std::string_view data, bool is_stderr) {
    LOG(INFO) << "IO driver " << (is_stderr ? "stderr" : "stdout") << ": "
              << data;
  });
}
