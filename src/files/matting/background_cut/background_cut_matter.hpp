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
  int downscale_factor;
  int median_blur_kernel_size;
  constexpr static double contrast_term_scale{1};// lambda in equation (1)

  const SegmentationResult prev_segmentation_res;
  ColorModel color_model;

public:
  explicit BackgroundCutMatter(
      const cv::Mat &background, int downscale_factor = 2,
      int median_blur_kernel_size = 21, double color_model_mix_factor = 0.25,
      int global_bg_model_num_components = 11,
      int global_fg_model_num_components = 5,
      double global_fg_model_fg_threshold = 0.05) noexcept
      : downscale_factor{downscale_factor},
        median_blur_kernel_size{median_blur_kernel_size},
        prev_segmentation_res{SegmentationResult::empty(
            background.rows / downscale_factor,
            background.cols / downscale_factor)},
        color_model{
            Image(std::move(background)).downscaled(downscale_factor),
            color_model_mix_factor, global_bg_model_num_components,
            global_fg_model_num_components, global_fg_model_fg_threshold} {};
  cv::Mat background_mask(const cv::Mat &video_frame) override;
};

#endif
