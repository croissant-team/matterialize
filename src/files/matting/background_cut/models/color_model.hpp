#ifndef MATTERIALIZE_COLOR_MODEL_HPP
#define MATTERIALIZE_COLOR_MODEL_HPP

#include "../../matter_config.hpp"
#include "../types/color_term.hpp"
#include "../types/image.hpp"
#include "global_bg_model.hpp"
#include "global_fg_model.hpp"
#include "pixel_bg_model.hpp"



class ColorModel {
private:
  MatterConfig &config;
  const GlobalBgModel global_bg_model;
  const PixelBgModel pixel_bg_model;

  // the global foreground model is learnt frame-by-frame and thus shouldn't be
  // const
  GlobalFgModel global_fg_model;

public:
  explicit ColorModel(
      const Image &bg_image, MatterConfig &config, int global_fg_model_num_components,
      double global_fg_model_fg_threshold);

  [[nodiscard]] Probability mix_probs(const Image &img) const;

  [[nodiscard]] ColorTerm color_term(
      const Image &img, const SegmentationResult &prev_segmentation_result);
};

#endif// MATTERIALIZE_COLOR_MODEL_HPP
