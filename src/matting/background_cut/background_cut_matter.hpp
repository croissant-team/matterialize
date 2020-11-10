#ifndef MATTERIALIZE_BACKGROUND_CUT_MATTER_HPP
#define MATTERIALIZE_BACKGROUND_CUT_MATTER_HPP

#include "../matter.hpp"
#include "models/color_model.hpp"
#include "models/global_bg_model.hpp"
#include "models/global_fg_model.hpp"
#include "models/pixel_bg_model.hpp"
#include "types/image.hpp"

#include <iostream>
#include <utility>

#include <opencv2/core.hpp>

class BackgroundCutMatter : public IMatter {
private:
  constexpr static int downscale_factor{2};
  constexpr static int median_blur_kernel_size{11};
  constexpr static double contrast_term_scale{1};// lambda in equation (1)

  const SegmentationResult prev_segmentation_res;
  ColorModel color_model;

public:
  explicit BackgroundCutMatter(const cv::Mat &background) noexcept
      : color_model{Image(std::move(background)).downscaled(downscale_factor)},
        prev_segmentation_res{SegmentationResult::empty(
            background.rows / downscale_factor,
            background.cols / downscale_factor)} {};
  cv::Mat background_mask(const cv::Mat &video_frame) override;
};

#endif
