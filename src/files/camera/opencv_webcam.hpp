#ifndef MATTERIALIZE_OPENCV_WEBCAM_HPP
#define MATTERIALIZE_OPENCV_WEBCAM_HPP

#include <mutex>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

class OpenCVWebcam {
  friend class OpenCVWebcamControls;

private:
  const int width;
  const int height;
  int device;
  cv::VideoCapture capture;
  std::mutex device_mutex;

public:
  OpenCVWebcam(int device, int width, int height)
      : device{device},
        width{width},
        height{height},
        capture{cv::VideoCapture{}},
        device_mutex() {}

  void start();

  void stop();

  void changeDevice(int new_device);

  cv::Mat grab();

  void roll(int num_grabs);

  long double fps(int samples);
};

#endif
