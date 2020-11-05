#ifndef MATTERIALIZE_BACKGROUND_CUT_MATTER_HPP
#define MATTERIALIZE_BACKGROUND_CUT_MATTER_HPP

#include "../matter.hpp"
#include "models/global_bg_model.hpp"
#include "models/pixel_bg_model.hpp"
#include "types/image.hpp"

#include <iostream>
#include <utility>

#include <opencv2/core.hpp>

class BackgroundCutMatter : public IMatter {
public:
  //const PixelBgModel pixel_bg_model;
  const GlobalBgModel color_bg_model;
  const double fg_thresh;
  const int median_blur_kernel_size;

  explicit BackgroundCutMatter(const cv::Mat &&background) noexcept
      : /*pixel_bg_model{PixelBgModel(Image(cv::Mat(background)))},*/ color_bg_model{GlobalBgModel(Image(cv::Mat(background)))}, fg_thresh{1E-30},
        median_blur_kernel_size{25} {}

  cv::Mat background_mask(const cv::Mat &video_frame) override;
};

#endif
