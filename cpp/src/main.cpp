#include "camera/opencv_webcam.hpp"

#include <iostream>

#include <opencv2/highgui.hpp>

int main() {
  std::cout << "Matterialize\n";

  OpenCVWebcam webcam(0, 1280, 720);

  webcam.start();

  cv::namedWindow("Webcam", cv::WINDOW_NORMAL);
  cv::resizeWindow("Webcam", 1280, 720);

  while (true) {
    const cv::Mat &frame{webcam.grab()};

    if (frame.empty()) {
      break;
    }

    cv::imshow("Webcam", frame);

    if (cv::waitKey(10) == 27) {
      break;
    }
  }
}
