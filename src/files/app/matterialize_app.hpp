#ifndef MATTERIALIZE_SRC_FILES_APP_MATTERIALIZE_APP_HPP
#define MATTERIALIZE_SRC_FILES_APP_MATTERIALIZE_APP_HPP

#include "../camera/fake_webcam.hpp"
#include "../camera/opencv_webcam_controls.hpp"
#include "../util/cleanup_handler.hpp"
#include "../util/video_devices.hpp"

#include <atomic>

class MatterializeApp {
private:
  static constexpr int width{640};
  static constexpr int height{480};
  static constexpr int input_device{0};
  static constexpr int preview_device{MATTERIALIZE_PREVIEW};
  static constexpr int output_device{MATTERIALIZE_CAM};
  static constexpr int num_void_frames{60};
  static constexpr int num_estimation_frames{30};

  CleanupHandler &cleanup_handler;
  std::atomic_bool running{true};

  OpenCVWebcam input_cam{input_device, width, height};
  OpenCVWebcamControls input_cam_controls{input_cam};
  FakeWebcam output_cam{output_device, width, height};
  FakeWebcam preview_cam{preview_device, width, height};

  IMatter *matter = nullptr;
  MatterMode initial_matter_mode = &MatterModes::NONE;
  BackgroundSettings bg_settings{width, height};
  std::mutex matter_mutex;
  std::unique_lock<std::mutex> matter_lock{matter_mutex, std::defer_lock};

  Pistache::Port port{9000};
  int thr{2};
  Pistache::Address addr{Pistache::Ipv4::any(), port};
  ServerEndpoint server{
      addr,
      running,
      input_cam,
      input_cam_controls,
      matter,
      initial_matter_mode,
      matter_mutex,
      bg_settings};
  std::thread server_thread;

  queue<cv::Mat> frame_queue;
  std::mutex frame_queue_mutex;
  std::condition_variable new_entry;
  bool has_new_entry{false};
  VideoRecorder video_recorder{0, cv::Size(0, 0), nullptr, nullptr, nullptr, nullptr};
  std::thread recording_thread;

  long estimate_fps();
  bool process_frame();

public:
  MatterializeApp(CleanupHandler &cleanup_handler)
      : cleanup_handler{cleanup_handler} {
    // Setup cameras
    try {
      input_cam.start();
    } catch (const std::exception &e) { std::cout << e.what() << "\n"; }
    cleanup_handler.set_input_webcam(&input_cam);

    input_cam_controls.enable_automatic();
    cleanup_handler.set_camera_controls(&input_cam_controls);

    output_cam.start();
    cleanup_handler.set_output_webcam(&output_cam);

    preview_cam.start();
    cleanup_handler.set_preview_webcam(&preview_cam);

    // Setup clean plate
    input_cam.roll(num_void_frames);
    input_cam_controls.disable_automatic();

    // Setup server
    server.init(thr);
    server_thread = std::thread(&ServerEndpoint::start, &server);
    cleanup_handler.set_server_endpoint(&server);
    cleanup_handler.set_server_thread(&server_thread);

    // Setup recording thread
    video_recorder = VideoRecorder(
        estimate_fps(),
        output_cam.get_size(),
        &frame_queue,
        &frame_queue_mutex,
        &new_entry,
        &has_new_entry);
    recording_thread = std::thread(&VideoRecorder::record, video_recorder);
    cleanup_handler.set_frame_queue(&frame_queue);
    cleanup_handler.set_frame_queue_mutex(&frame_queue_mutex);
    cleanup_handler.set_condition_variable(&new_entry);
    cleanup_handler.set_condition_boolean(&has_new_entry);
    cleanup_handler.set_recording_thread(&recording_thread);
  }

  void run();
};

#endif
