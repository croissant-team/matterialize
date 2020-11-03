#include "camera/fake_webcam.hpp"
#include "camera/opencv_webcam.hpp"
#include "camera/webcam_controls.hpp"
#include "util/converter.hpp"

#include <iostream>
#include <v4l2cpp/V4l2Capture.h>

using namespace std;

int main() {
  std::cout << "Matterialize\n";

  OpenCVWebcam webcam(0, 640, 480);
  WebcamControls controls(0);
  webcam.start();
  FakeWebcam output(2, webcam);
  output.start();
  controls.disable_automatic_controls();

  while (true) {
    const cv::Mat &frame{webcam.grab()};

    if (frame.empty()) {
      break;
    }

    output.write(frame);
  }

  webcam.stop();
  output.stop();
}
