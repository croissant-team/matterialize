#ifndef MATTERIALIZE_OPENCV_WEBCAM_CONTROLS_HPP
#define MATTERIALIZE_OPENCV_WEBCAM_CONTROLS_HPP

#include "opencv_webcam.hpp"

class OpenCVWebcamControls {
private:
  OpenCVWebcam &webcam;

public:
  void enable_automatic();

  void disable_automatic();

  explicit OpenCVWebcamControls(OpenCVWebcam &webcam) : webcam{webcam} {
    enable_automatic();
  }
};

#endif
