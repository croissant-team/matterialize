#ifndef MATTERIALIZE_BACKGROUND_CUT_MATTER_HPP
#define MATTERIALIZE_BACKGROUND_CUT_MATTER_HPP

#include "../matter.hpp"
#include "models/pixel_bg_model.hpp"
#include "types/image.hpp"

#include <iostream>
#include <utility>

#include <opencv2/core.hpp>

class BackgroundCutMatter : public IMatter {
public:
  const PixelBGModel bg_model;
  const double fg_thresh;
  const int median_blur_kernel_size;

  explicit BackgroundCutMatter(const cv::Mat &&background) noexcept
      : bg_model{PixelBGModel(Image(cv::Mat(background)))}, fg_thresh{1E-15},
        median_blur_kernel_size{21} {}

  cv::Mat background_mask(const cv::Mat &video_frame) override;
};

#endif
