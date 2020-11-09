#ifndef MATTERIALIZE_OPENCV_MATTER_HPP
#define MATTERIALIZE_OPENCV_MATTER_HPP

#include "matter.hpp"

#include <opencv2/core.hpp>
#include <opencv2/video.hpp>

class OpenCVMatter : public IMatter {
private:
  cv::Ptr<cv::BackgroundSubtractor> background_subtractor{
      cv::createBackgroundSubtractorKNN()};

public:
  cv::Mat background_mask(const cv::Mat &video_frame) override;
};

#endif
