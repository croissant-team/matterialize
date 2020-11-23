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
  constexpr static MatterConfigField downscale_factor{
      "downscale_factor", "2", true};
  constexpr static MatterConfigField median_blur_kernel_size{
      "median_blur_kernel_size", "21", false};
  constexpr static MatterConfigField color_model_mix_factor{
      "color_model_mix_factor", "0.25", false};
  constexpr static MatterConfigField global_bg_model_num_components{
      "global_bg_model_num_components", "11", true};
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
        stoi(config.get(config_fields::downscale_factor)));
  }
  [[nodiscard]] vector<MatterConfigField> config_fields() const override {
    using namespace background_cut::config_fields;
    return {
        downscale_factor,
        median_blur_kernel_size,
        color_model_mix_factor,
        global_bg_model_num_components};
  }

  [[nodiscard]] bool requires_clean_plate() const override {
    return true;
  }
};

#endif
