#include "opencv_webcam.hpp"

#include <chrono>
#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

void OpenCVWebcam::start() {
  capture.open(device, cv::CAP_V4L);
  capture.set(cv::CAP_PROP_FRAME_WIDTH, width);
  capture.set(cv::CAP_PROP_FRAME_HEIGHT, height);
  //  capture.set(cv::CAP_PROP_BUFFERSIZE, 3);
}

void OpenCVWebcam::stop() {
  capture.release();
}

cv::Mat OpenCVWebcam::grab() {
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
