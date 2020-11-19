#include "video_recorder.hpp"

#include <thread>

void VideoRecorder::record() {
  std::unique_lock frame_queue_lock{frame_queue_mutex, std::defer_lock};
  while (true) {
    new_entry.wait(frame_queue_lock, [this] { return has_new_entry; });
    if (frame_queue.empty()) {
      break;
    }
    has_new_entry = false;
    video_writer.write(frame_queue.back());
    frame_queue.pop();
  }
}
