#ifndef MATTERIALIZE_SRC_FILES_SERVER_RECORDING_HANDLER_HPP
#define MATTERIALIZE_SRC_FILES_SERVER_RECORDING_HANDLER_HPP

#include <pistache/router.h>
#include <pwd.h>
#include <sys/stat.h>

class RecordingHandler {
public:
  RecordingHandler() {
    struct stat buffer{};
    if (stat(recordings_path.c_str(), &buffer) != 0) {
      assert(std::system(("mkdir " + recordings_path).c_str()) == EXIT_SUCCESS);
    }
  }

  void setup_routes(Pistache::Rest::Router &router);
  void cleanup();

private:
  bool recording{false};
  pid_t recording_pid{};
  std::string recordings_path{
      std::string(getpwuid(getuid())->pw_dir) + "/.matterialize/recordings/"};

  void start_recording(
      const Pistache::Rest::Request &request,
      Pistache::Http::ResponseWriter response);
  void stop_recording(
      const Pistache::Rest::Request &request,
      Pistache::Http::ResponseWriter response);
};

#endif
