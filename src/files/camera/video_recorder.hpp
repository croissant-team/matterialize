#ifndef MATTERIALIZE_SRC_FILES_CAMERA_VIDEO_RECORDER_HPP
#define MATTERIALIZE_SRC_FILES_CAMERA_VIDEO_RECORDER_HPP

#include "fake_webcam.hpp"

#include <condition_variable>
#include <ctime>
#include <opencv2/opencv.hpp>
#include <pwd.h>
#include <queue>
#include <unistd.h>

class VideoRecorder {
private:
  cv::VideoWriter video_writer;
  bool recording{true};
  int fps;
  cv::Size frame_size;
  queue<cv::Mat> *frame_queue;
  std::mutex *frame_queue_mutex;
  std::condition_variable *new_entry;
  bool *has_new_entry;
  std::string recordings_path{
      std::string(getpwuid(getuid())->pw_dir) + "/.matterialize/recordings/"};

  bool directory_exists();

public:
  VideoRecorder(
      int fps, cv::Size size, queue<cv::Mat> *frame_queue,
      std::mutex *frame_queue_mutex, std::condition_variable *new_entry,
      bool *has_new_entry)
      : fps{fps},
        frame_size{size},
        frame_queue{frame_queue},
        frame_queue_mutex{frame_queue_mutex},
        new_entry{new_entry},
        has_new_entry{has_new_entry} {}

  void record();
};

#endif
