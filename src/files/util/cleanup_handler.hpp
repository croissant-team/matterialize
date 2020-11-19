#ifndef MATTERIALIZE_SRC_FILES_UTIL_CLEANUP_HANDLER_HPP
#define MATTERIALIZE_SRC_FILES_UTIL_CLEANUP_HANDLER_HPP

#include "../camera/fake_webcam.hpp"
#include "../camera/opencv_webcam_controls.hpp"
#include "../camera/video_recorder.hpp"
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
  queue<cv::Mat> *frame_queue{NULL};
  std::mutex *frame_queue_mutex{NULL};
  std::condition_variable *new_entry{NULL};
  bool *has_new_entry{NULL};

  void finish_recording();

public:
  void perform_cleanup_actions(int signal);
  void set_camera_controls(OpenCVWebcamControls *new_controls);
  void set_server_endpoint(ServerEndpoint *new_server);
  void set_server_thread(std::thread *new_server_thread);
  void set_input_webcam(OpenCVWebcam *new_webcam);
  void set_output_webcam(FakeWebcam *new_output);
  void set_preview_webcam(FakeWebcam *new_preview);
  void set_frame_queue(queue<cv::Mat> *new_frame_queue);
  void set_frame_queue_mutex(std::mutex *new_frame_queue_mutex);
  void set_condition_variable(std::condition_variable *new_condition_variable);
  void set_condition_boolean(bool *new_condition_boolean);
};

#endif
