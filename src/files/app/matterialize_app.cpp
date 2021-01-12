#include "matterialize_app.hpp"

void MatterializeApp::run() {
  while (running && process_frame()) {}
  cleanup_handler.perform_cleanup_actions(0);
}

bool MatterializeApp::process_frame() {
  if (input_cam.isAvailable) {
    auto start{std::chrono::system_clock::now()};

    matter_lock.lock();

    const cv::Mat frame{input_cam.grab()};
    if (frame.empty()) {
      return false;
    }

    cv::Mat result{matter->change_background(
        frame, bg_settings.generate_background(frame))};

    matter_lock.unlock();

    output_cam.write(result);
    preview_cam.write(result);

    int elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                         std::chrono::system_clock::now() - start)
                         .count();
    cout << elapsed_ms << '\n';
  } else {
    cout << "Selected device not available" << '\n';
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  return true;
}
MatterializeApp::MatterializeApp(CleanupHandler &cleanup_handler)
    : cleanup_handler{cleanup_handler} {
  // Setup cameras
  try {
    input_cam.start();
    input_cam_controls.enable_automatic();
  } catch (const std::invalid_argument &e) { std::cout << e.what() << "\n"; }

  cleanup_handler.set_input_webcam(&input_cam);
  cleanup_handler.set_camera_controls(&input_cam_controls);

  output_cam.start();
  cleanup_handler.set_output_webcam(&output_cam);

  preview_cam.start();
  cleanup_handler.set_preview_webcam(&preview_cam);

  // Setup clean plate
  if (input_cam.isAvailable) {
    input_cam.roll(num_void_frames);
    input_cam_controls.disable_automatic();
  }

  // Setup server
  server.init(thr);
  server_thread = std::thread(&ServerEndpoint::start, &server);
  cleanup_handler.set_server_endpoint(&server);
  cleanup_handler.set_server_thread(&server_thread);
}
