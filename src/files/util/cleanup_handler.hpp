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
  OpenCVWebcamControls *controls{nullptr};
  ServerEndpoint *server{nullptr};
  std::thread *server_thread{nullptr};
  OpenCVWebcam *input{nullptr};
  FakeWebcam *output{nullptr};
  FakeWebcam *preview{nullptr};
  queue<cv::Mat> *frame_queue{nullptr};
  std::mutex *frame_queue_mutex{nullptr};
  std::condition_variable *new_entry{nullptr};
  bool *has_new_entry{nullptr};
  std::thread *recording_thread{nullptr};

  void reenable_automatic_controls();
  void shutdown_server();
  void stop_cameras();
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
  void set_recording_thread(std::thread *new_recording_thread);
};

#endif
