#include "camera/fake_webcam.hpp"
#include "camera/opencv_webcam.hpp"
#include "camera/opencv_webcam_controls.hpp"
#include "util/converter.hpp"

#include <iostream>
#include <v4l2cpp/V4l2Capture.h>

using namespace std;

int main() {
  std::cout << "Matterialize\n";

  OpenCVWebcam webcam(0, 640, 480);
  webcam.start();
  // The webcam must be started before the webcam controls are initialised
  OpenCVWebcamControls opencv_controls(webcam);
  // The webcam must be started before the fake webcam is initialised
  FakeWebcam output(2, webcam);
  output.start();
  // The automatic controls should be disabled after the fake cam is initialised to give time for the automatic values to settle
  opencv_controls.disable_automatic();

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
