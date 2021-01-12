#ifndef MATTERIALIZE_SRC_FILES_APP_MATTERIALIZE_APP_HPP
#define MATTERIALIZE_SRC_FILES_APP_MATTERIALIZE_APP_HPP

#include "../camera/fake_webcam.hpp"
#include "../camera/opencv_webcam_controls.hpp"
#include "../util/cleanup_handler.hpp"
#include "../util/video_devices.hpp"

#include <atomic>
#include <thread>

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

  bool process_frame();

public:
  MatterializeApp(CleanupHandler &cleanup_handler);

  void run();
};

#endif
