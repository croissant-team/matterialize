#include "opencv_webcam.hpp"

#include <chrono>
#include <iostream>
#include <stdexcept>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

void OpenCVWebcam::start() {
  capture.open(device, cv::CAP_V4L);

  if (!capture.isOpened()) {
    throw std::invalid_argument("Device number not available");
  }

  capture.set(cv::CAP_PROP_FRAME_WIDTH, width);
  capture.set(cv::CAP_PROP_FRAME_HEIGHT, height);
  //  capture.set(cv::CAP_PROP_BUFFERSIZE, 3);
}

void OpenCVWebcam::stop() {
  capture.release();
}

void OpenCVWebcam::changeDevice(int new_device) {
  cv::VideoCapture attempt_capture;
  attempt_capture.open(new_device);

  if (!attempt_capture.isOpened()) {
    throw std::invalid_argument("Device number not available");
  }

  attempt_capture.release();

  std::scoped_lock lock(device_mutex);

  stop();
  device = new_device;
  start();
}

cv::Mat OpenCVWebcam::grab() {
  std::scoped_lock lock(device_mutex);
  cv::Mat frame{};

  capture >> frame;

  return frame;
}

long double OpenCVWebcam::fps(int samples) {
  std::cout << "Taking " << samples << " samples to estimate FPS...\n";

  long sum{};

  for (int i{}; i < samples; ++i) {
    auto start{std::chrono::system_clock::now()};

    grab();

    auto end{std::chrono::system_clock::now()};

    sum += std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
               .count();
  }

  return 1000 / (sum / static_cast<long double>(samples));
}
