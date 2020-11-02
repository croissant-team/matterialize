#include "camera/opencv_webcam.hpp"

#include <iostream>

#include <opencv2/highgui.hpp>
#include <v4l2cpp/V4l2Capture.h>
#include <v4l2cpp/V4l2Device.h>

int main() {
  std::cout << "Matterialize\n";

  /*OpenCVWebcam webcam(0, 1280, 720);

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
  }*/

  V4L2DeviceParameters param(
      "/dev/video0", V4L2_PIX_FMT_YUYV, 640, 480, 25, true);
  V4l2Capture *videoCapture =
      V4l2Capture::create(param, V4l2Access::IOTYPE_MMAP);
}
