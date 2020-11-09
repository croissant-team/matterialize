#ifndef MATTERIALIZE_COLOR_MODEL_HPP
#define MATTERIALIZE_COLOR_MODEL_HPP

#include "../types/color_term.hpp"
#include "../types/image.hpp"
#include "global_bg_model.hpp"
#include "global_fg_model.hpp"
#include "pixel_bg_model.hpp"

class ColorModel {
private:
  const double mix_factor;
  const GlobalBgModel global_bg_model;
  const PixelBgModel pixel_bg_model;

  // the global foreground model is learnt frame-by-frame and thus shouldn't be const
  GlobalFgModel global_fg_model;

public:
  explicit ColorModel(const Image &bg_image)
      : mix_factor{0.5}, global_bg_model{bg_image}, pixel_bg_model{bg_image},
        global_fg_model{global_bg_model} {
    assert(0 <= mix_factor && mix_factor <= 1);
  }

  [[nodiscard]] Probability mix_probs(const Image &img) const;

  [[nodiscard]] ColorTerm color_term(
      const Image &img, const SegmentationResult &prev_segmentation_result);
};

#endif//MATTERIALIZE_COLOR_MODEL_HPP
