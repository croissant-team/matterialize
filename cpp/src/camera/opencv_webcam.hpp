#ifndef MATTERIALIZE_OPENCV_WEBCAM_HPP
#define MATTERIALIZE_OPENCV_WEBCAM_HPP

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

class OpenCVWebcam {
  friend class OpenCVWebcamControls;

private:
  const int device;
  const int width;
  const int height;
  cv::VideoCapture capture;

public:
  OpenCVWebcam(int device, int width, int height)
      : device{device}, width{width}, height{height}, capture{
                                                          cv::VideoCapture{}} {}

  void start();

  void stop();

  cv::Mat grab();

  long double fps(int samples);
};

#endif
