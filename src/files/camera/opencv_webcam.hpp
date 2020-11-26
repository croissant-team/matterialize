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
  bool isAvailable;

  OpenCVWebcam(int device, int width, int height)
      : width{width},
        height{height},
        device{device},
        capture{cv::VideoCapture{}},
        device_mutex(),
        isAvailable{false} {}

  void start();

  void stop();

  void changeDevice(int new_device);

  cv::Mat grab();

  void roll(int num_grabs);

  long double fps(int samples);

  int get_device_num() const;
};

#endif
