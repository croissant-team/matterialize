#ifndef MATTERIALIZE_SRC_FILES_CAMERA_VIDEO_RECORDER_HPP
#define MATTERIALIZE_SRC_FILES_CAMERA_VIDEO_RECORDER_HPP

#include "fake_webcam.hpp"

#include <condition_variable>
#include <opencv2/opencv.hpp>
#include <queue>

class VideoRecorder {
private:
  cv::VideoWriter video_writer;
  bool recording{true};
  queue<cv::Mat> &frame_queue;
  std::mutex &frame_queue_mutex;
  std::condition_variable &new_entry;
  bool &has_new_entry;

public:
  VideoRecorder(
      int fps, cv::Size size, queue<cv::Mat> &frame_queue,
      std::mutex &frame_queue_mutex, std::condition_variable &new_entry,
      bool &has_new_entry)
      : video_writer{cv::VideoWriter(
            "test.avi", cv::VideoWriter::fourcc('X', 'V', 'I', 'D'), fps,
            size)},
        frame_queue{frame_queue},
        frame_queue_mutex{frame_queue_mutex},
        new_entry{new_entry},
        has_new_entry{has_new_entry} {}

  void record();
};

#endif
