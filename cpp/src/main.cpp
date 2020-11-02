#include "camera/opencv_webcam.hpp"
#include "matting/background_cut/background_cut_matter.hpp"
#include "opencv2/imgproc.hpp"

#include <iostream>

#include <opencv2/highgui.hpp>

int main() {
  std::cout << "Matterialize\n";

  OpenCVWebcam webcam(0, 640, 360);

  webcam.start();

  cv::namedWindow("Webcam", cv::WINDOW_NORMAL);
  cv::resizeWindow("Webcam", 1280, 720);

  const cv::Mat &clean{webcam.grab()};
  auto matter = BackgroundCutMatter((const cv::Mat &&) clean);

  while (true) {
    const cv::Mat &frame{webcam.grab()};

    const cv::Mat &mask{matter.green_screen(frame)};

    if (frame.empty()) {
      break;
    }

    cv::imshow("Webcam", mask);

    if (cv::waitKey(10) == 27) {
      break;
    }
  }
}
