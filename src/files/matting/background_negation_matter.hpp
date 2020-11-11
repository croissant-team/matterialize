#ifndef MATTERIALIZE_BACKGROUND_NEGATION_MATTER_HPP
#define MATTERIALIZE_BACKGROUND_NEGATION_MATTER_HPP

#include "matter.hpp"

#include <utility>

#include <opencv2/core.hpp>

class BackgroundNegationMatter : public IMatter {
private:
  const cv::Mat background;

public:
  explicit BackgroundNegationMatter(cv::Mat background) noexcept
      : background{std::move(background)} {}

  cv::Mat background_mask(const cv::Mat &video_frame) override;
};

#endif
