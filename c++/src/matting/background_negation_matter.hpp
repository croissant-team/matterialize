#ifndef MATTERIALIZE_BACKGROUND_NEGATION_MATTER_HPP
#define MATTERIALIZE_BACKGROUND_NEGATION_MATTER_HPP

#include "matter.hpp"

#include <opencv2/core.hpp>

class BackgroundNegationMatter : public IMatter {
private:
  const cv::Mat &background;

public:
  explicit BackgroundNegationMatter(const cv::Mat &background) : background{background} {}

  cv::Mat background_mask(const cv::Mat &video_frame) override;
};

#endif
