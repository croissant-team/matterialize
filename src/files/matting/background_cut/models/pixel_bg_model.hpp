#ifndef MATTERIALIZE_PIXEL_BG_MODEL_HPP
#define MATTERIALIZE_PIXEL_BG_MODEL_HPP

#include "../types/image.hpp"
#include "../types/pixel_variances.hpp"
#include "../types/probability.hpp"

#include <opencv2/core.hpp>

class PixelBgModel {
private:
  constexpr static double magic_scaling = (0.05 / 1E-15);
  const PixelVariance per_pixel_variance;
  const Image bg_image;

public:
  explicit PixelBgModel(const Image &bg_image)
      : per_pixel_variance{bg_image.get_pixel_variances()},
        bg_image{bg_image} {}

  [[nodiscard]] double pdf(double l2, double det) const;

  [[nodiscard]] Probability get_pixel_probability(const Image &image) const;
};

#endif
