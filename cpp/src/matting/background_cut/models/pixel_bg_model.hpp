#ifndef MATTERIALIZE_PIXEL_BG_MODEL_HPP
#define MATTERIALIZE_PIXEL_BG_MODEL_HPP

#include "../types/flat_image.hpp"
#include "../types/image.hpp"
#include "../types/pixel_variances.hpp"
#include "../types/probability.hpp"

#include <opencv2/core.hpp>

class PixelBGModel {
private:
  const PixelVariance per_pixel_variance;
  const FlatImage flat_bg_image;

public:
  explicit PixelBGModel(const Image &bg_image)
      : per_pixel_variance{bg_image.get_pixel_variances()},
        flat_bg_image{bg_image.flattened()} {}

  [[nodiscard]] double pdf(double l2, double det) const;

  [[nodiscard]] Probability get_pixel_probability(const FlatImage &image) const;
};

#endif
