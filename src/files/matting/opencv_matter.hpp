#ifndef MATTERIALIZE_OPENCV_MATTER_HPP
#define MATTERIALIZE_OPENCV_MATTER_HPP

#include "matter.hpp"
#include "matter_mode.hpp"

#include <opencv2/core.hpp>
#include <opencv2/video.hpp>

class OpenCVMatter : public IMatter {
private:
  cv::Ptr<cv::BackgroundSubtractor> background_subtractor{
      cv::createBackgroundSubtractorKNN()};

public:
  cv::Mat background_mask(const cv::Mat &video_frame) override;
};

class OpenCVMode : public IMatterMode {
public:
  [[nodiscard]] const string name() const override {
    return "OpenCV";
  }

  IMatter *
  init_matter(MatterInitData &data, MatterConfig &config) const override {
    return new OpenCVMatter();
  }

  [[nodiscard]] vector<const IMatterConfigField *>
  config_fields() const override {
    return {};
  }

  [[nodiscard]] bool requires_clean_plate() const override {
    return false;
  }
};

#endif
