#include "matterialize_app.hpp"

void MatterializeApp::run() {
  while (running && process_frame()) {}
  cleanup_handler.perform_cleanup_actions(0);
}

bool MatterializeApp::process_frame() {
  auto start{std::chrono::system_clock::now()};

  matter_lock.lock();
  const cv::Mat frame{input_cam.grab()};
  if (frame.empty()) {
    return false;
  }

  cv::Mat result{
      matter->change_background(frame, bg_settings.generate_background(frame))};
  matter_lock.unlock();

  output_cam.write(result);
  preview_cam.write(result);

  int elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                       std::chrono::system_clock::now() - start)
                       .count();
  cout << elapsed_ms << '\n';
  return true;
}
