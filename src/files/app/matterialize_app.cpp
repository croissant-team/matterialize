#include "matterialize_app.hpp"

void MatterializeApp::run() {
  while (running && process_frame()) {}
  cleanup_handler.perform_cleanup_actions(0);
}

long MatterializeApp::estimate_fps() {
  auto start = std::chrono::system_clock::now();
  for (int i = 0; i < num_estimation_frames; i++) {
    process_frame();
  }
  return (
      30000 / std::chrono::duration_cast<std::chrono::milliseconds>(
                  std::chrono::system_clock::now() - start)
                  .count());
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
  frame_queue_mutex.lock();
  frame_queue.push(result);
  has_new_entry = true;
  cout << frame_queue.size() << "\n";
  frame_queue_mutex.unlock();
  new_entry.notify_one();
  int elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                       std::chrono::system_clock::now() - start)
                       .count();
  cout << elapsed_ms << '\n';
  return true;
}
