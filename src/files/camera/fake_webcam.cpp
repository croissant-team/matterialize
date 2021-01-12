#include "fake_webcam.hpp"
#include "../util/converter.hpp"

using namespace std;

void FakeWebcam::write(const cv::Mat &frame) {
  vector<uchar> converted = Converter::to_YUYV(frame);
  output->write((char *) &converted[0], converted.size());
}

void FakeWebcam::stop() {
  output->stop();
}

void FakeWebcam::start() {
  output->start();
}

cv::Size FakeWebcam::get_size() {
  return cv::Size(width, height);
}

FakeWebcam::FakeWebcam(int device, int width, int height)
    : device{device}, width{width}, height{height}, fps{30} {
  string device_str = "/dev/video" + to_string(device);

  V4L2DeviceParameters params(
      device_str.c_str(), V4L2_PIX_FMT_YUYV, width, height, fps);

  output = V4l2Output::create(params, V4l2Access::IOTYPE_READWRITE);
}
