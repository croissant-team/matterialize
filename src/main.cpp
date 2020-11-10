#include "camera/fake_webcam.hpp"
#include "camera/opencv_webcam.hpp"
#include "camera/opencv_webcam_controls.hpp"
#include "matting/background_cut/background_cut_matter.hpp"
#include "util/converter.hpp"
#include <chrono>
#include <iostream>
#include <v4l2cpp/V4l2Capture.h>

constexpr int width{640};
constexpr int height{480};
constexpr int input_device{0};
constexpr int output_device{2};
constexpr int num_void_frames{60};

int main() {
  std::cout << "Matterialize\n";
  OpenCVWebcam webcam(input_device, width, height);
  webcam.start();
  // The webcam must be started before the webcam controls are initialised
  OpenCVWebcamControls opencv_controls(webcam);
  // The webcam must be started before the fake webcam is initialised
  FakeWebcam output(output_device, width, height);
  output.start();
  // Grab some frames to ensure exposure/white balance are setup correctly
  for (int i{0}; i < num_void_frames; ++i) {
    webcam.grab();
  }
  BackgroundCutMatter matter(webcam.grab());
  // The automatic controls should be disabled after the fake cam is initialised
  // to give time for the automatic values to settle
  opencv_controls.disable_automatic();
  while (true) {
    auto start{std::chrono::system_clock::now()};
    const cv::Mat frame{webcam.grab()};
    if (frame.empty()) {
      break;
    }
    cv::Mat result{matter.green_screen(frame)};
    output.write(result);
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now() - start)
        .count()
              << '\n';
  }
  webcam.stop();
  output.stop();
}