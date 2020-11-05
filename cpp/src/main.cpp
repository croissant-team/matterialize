#include "camera/fake_webcam.hpp"
#include "camera/opencv_webcam.hpp"
#include "camera/opencv_webcam_controls.hpp"
#include "util/converter.hpp"

#include <iostream>
#include <v4l2cpp/V4l2Capture.h>

using namespace std;

#define WIDTH 640
#define HEIGHT 480
#define INPUT_DEVICE 0
#define OUTPUT_DEVICE 2

int main() {
  std::cout << "Matterialize\n";

  OpenCVWebcam webcam(INPUT_DEVICE, WIDTH, HEIGHT);
  webcam.start();
  // The webcam must be started before the webcam controls are initialised
  OpenCVWebcamControls opencv_controls(webcam);
  // The webcam must be started before the fake webcam is initialised
  FakeWebcam output(OUTPUT_DEVICE, WIDTH, HEIGHT);
  output.start();

  // Grab some frames to ensure exposure/white balance are setup correctly
  int num_void_frames{60};
  for (int i{0}; i < num_void_frames; ++i) {
    webcam.grab();
  }

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
