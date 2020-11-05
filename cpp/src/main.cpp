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

  system("v4l2-ctl -d /dev/video0 --set-ctrl=exposure_auto=3");
  system("v4l2-ctl -d /dev/video0 --set-ctrl=white_balance_temperature_auto=1");

  for (int i = 0; i < 100; i++) {
    webcam.grab();
  }

  system("v4l2-ctl -d /dev/video0 --set-ctrl=exposure_auto=1");
  system("v4l2-ctl -d /dev/video0 --set-ctrl=white_balance_temperature_auto=0");

  const cv::Mat &clean{webcam.grab()};
  auto matter = BackgroundCutMatter((const cv::Mat &&) clean);

  while (true) {
    auto start = std::chrono::high_resolution_clock::now();
    const cv::Mat &frame{webcam.grab()};
    const cv::Mat &mask{matter.green_screen(frame)};

    if (frame.empty()) {
      break;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << "\n";
    
    cv::imshow("Webcam", mask);
    if (cv::waitKey(10) == 27) {
      break;
    }
  }
}
