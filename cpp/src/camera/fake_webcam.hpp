#ifndef MATTERIALIZE_FAKE_WEBCAM_HPP
#define MATTERIALIZE_FAKE_WEBCAM_HPP

#include "opencv_webcam.hpp"
#include <opencv2/core.hpp>
#include <string>
#include <v4l2cpp/V4l2Output.h>

using namespace std;

#define NUM_SAMPLES 60

class FakeWebcam {
private:
  const int device;
  const int width;
  const int height;
  V4l2Output *output;

public:
  FakeWebcam(int device, OpenCVWebcam mimic)
      : device{device}, width{mimic.width}, height{mimic.height} {
    string device_str = "/dev/video" + to_string(device);
    V4L2DeviceParameters params(
        device_str.c_str(), V4L2_PIX_FMT_YUYV, width, height,
        round(mimic.fps(NUM_SAMPLES)));
    output = V4l2Output::create(params, V4l2Access::IOTYPE_READWRITE);
  }

  void start();
  void stop();
  void write(const cv::Mat &);
};

#endif
