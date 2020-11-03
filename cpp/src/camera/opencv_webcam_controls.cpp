#include "opencv_webcam_controls.hpp"

#include <iostream>

using namespace std;

void OpenCVWebcamControls::enable_automatic() {
  cout << "Enabling automatic webcam controls\n";
  webcam.capture.set(cv::CAP_PROP_AUTO_EXPOSURE, 3);
  webcam.capture.set(cv::CAP_PROP_AUTO_WB, 1);
}

void OpenCVWebcamControls::disable_automatic() {
  cout << "Disabling automatic webcam controls\n";
  webcam.capture.set(cv::CAP_PROP_AUTO_EXPOSURE, 1);
  webcam.capture.set(cv::CAP_PROP_AUTO_WB, 0);
}
