#ifndef MATTERIALIZE_BACKGROUND_CUT_MATTER_HPP
#define MATTERIALIZE_BACKGROUND_CUT_MATTER_HPP

#include "../matter.hpp"
#include "../matter_mode.hpp"
#include "models/color_model.hpp"
#include "models/global_bg_model.hpp"
#include "models/global_fg_model.hpp"
#include "models/pixel_bg_model.hpp"
#include "types/image.hpp"

#include <iostream>
#include <utility>

#include <opencv2/core.hpp>

namespace background_cut::config_fields {
  const static MatterConfigFieldDouble downscale_factor{
      "Downscale Factor (lower: better quality)",
      "downscale_factor",
      true,
      2,
      1.0,
      4.0,
      1.0};
  const static MatterConfigFieldDouble median_blur_kernel_size{
      "Median Blur Kernel Size (lower: more detail)",
      "median_blur_kernel_size",
      false,
      21,
      1,
      49,
      2};
  const static MatterConfigFieldDouble color_model_mix_factor{
      "Colour Model Mix Factor (lower: more bias to per pixel model)",
      "color_model_mix_factor",
      false,
      0.35,
      0,
      1,
      0.01};
  const static MatterConfigFieldDouble global_bg_model_num_components{
      "Number of Components",
      "global_bg_model_num_components",
      true,
      10.0,
      5.0,
      15.0,
      1.0};
}// namespace background_cut::config_fields

class BackgroundCutMatter : public IMatter {
private:
  MatterConfig &config;
  int downscale_factor;
  constexpr static double contrast_term_scale{1};// lambda in equation (1)

  const SegmentationResult prev_segmentation_res;
  ColorModel color_model;

public:
  explicit BackgroundCutMatter(
      const cv::Mat &background, MatterConfig &config, int downscale_factor = 2,
      int global_fg_model_num_components = 5,
      double global_fg_model_fg_threshold = 0.05) noexcept;
  ;
  cv::Mat background_mask(const cv::Mat &video_frame) override;
};

class BackgroundCutMode : public IMatterMode {
public:
  [[nodiscard]] const string name() const override {
    return "Background Cut";
  }
  IMatter *
  init_matter(MatterInitData &data, MatterConfig &config) const override {
    using namespace background_cut;
    return new BackgroundCutMatter(
        data.clean_plate,
        config,
        (int) config.get(config_fields::downscale_factor));
  }
  [[nodiscard]] vector<const IMatterConfigField *>
  config_fields() const override {
    using namespace background_cut::config_fields;
    return {
        &downscale_factor,
        &median_blur_kernel_size,
        &color_model_mix_factor,
        &global_bg_model_num_components};
  }

  [[nodiscard]] bool requires_clean_plate() const override {
    return true;
  }
};

#endif
