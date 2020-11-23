#include "video_recorder.hpp"

#include <filesystem>
#include <sys/stat.h>
#include <thread>

bool VideoRecorder::directory_exists() {
  struct stat buffer {};
  return (stat(recordings_path.c_str(), &buffer) == 0);
}

void VideoRecorder::record() {
  if (!directory_exists()) {
    assert(std::system(("mkdir " + recordings_path).c_str()));
  }
  time_t raw_time;
  time(&raw_time);
  video_writer = cv::VideoWriter(
      (recordings_path + asctime(localtime(&raw_time))),
      cv::VideoWriter::fourcc('X', 'V', 'I', 'D'),
      fps,
      frame_size);
  std::unique_lock frame_queue_lock{*frame_queue_mutex, std::defer_lock};
  while (true) {
    new_entry->wait(frame_queue_lock, [this] { return *has_new_entry; });
    cout << "Recording | frames in queue: " << frame_queue->size() << "\n";
    if (frame_queue->empty()) {
      break;
    }
    video_writer.write(frame_queue->front());
    frame_queue->pop();
    *has_new_entry = false;
    new_entry->notify_one();
  }
  cout << "Recoding ended\n";
}
