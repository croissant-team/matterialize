#ifndef MATTERIALIZE_FACE_DETECTION_MATTER_HPP
#define MATTERIALIZE_FACE_DETECTION_MATTER_HPP

#include "matter.hpp"

#include <opencv2/core.hpp>
#include <opencv2/objdetect.hpp>

class FaceDetectionMatter : public IMatter {
private:
  cv::CascadeClassifier classifier{
      cv::CascadeClassifier("resources/haarcascade_frontalface_default.xml")};

public:
  cv::Mat background_mask(const cv::Mat &video_frame) override;
};

#endif
