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
