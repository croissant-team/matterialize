#ifndef MATTERIALIZE_SRC_FILES_UTIL_CLEANUP_HANDLER_HPP
#define MATTERIALIZE_SRC_FILES_UTIL_CLEANUP_HANDLER_HPP

#include "../camera/fake_webcam.hpp"
#include "../camera/opencv_webcam_controls.hpp"
#include "../server/server_endpoint.hpp"

#include <csignal>
#include <thread>
#include <vector>

class CleanupHandler {
private:
  OpenCVWebcamControls *controls{NULL};
  ServerEndpoint *server{NULL};
  std::thread *server_thread{NULL};
  OpenCVWebcam *input{NULL};
  FakeWebcam *output{NULL};
  FakeWebcam *preview{NULL};

public:
  void perform_cleanup_actions(int signal);
  void set_camera_controls(OpenCVWebcamControls *new_controls);
  void set_server_endpoint(ServerEndpoint *new_server);
  void set_server_thread(std::thread *new_server_thread);
  void set_input_webcam(OpenCVWebcam *new_webcam);
  void set_output_webcam(FakeWebcam *new_output);
  void set_preview_webcam(FakeWebcam *new_preview);
};

#endif
