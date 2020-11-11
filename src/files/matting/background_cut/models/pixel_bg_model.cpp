#include "pixel_bg_model.hpp"

#include <cmath>
#include <utility>

#include <opencv2/core.hpp>

constexpr auto PI{3.14159265358979323846};

double PixelBgModel::pdf(double l2, double det) const {
  return std::exp(-(1 * l2) / 2) / std::sqrt(8 * PI * PI * PI * det);
}

Probability PixelBgModel::get_pixel_probability(const Image &image) const {
  cv::Mat probabilities(
      cv::Size2d(1.0, static_cast<double>(image.num_pixels())), CV_64FC1);

  for (int pixel{0}; pixel < image.num_pixels(); ++pixel) {
    double l2{0};
    double det{1.0};

    for (int channel{}; channel < 3; ++channel) {
      const double delta{
          static_cast<double>(image.get(pixel, channel)) -
          static_cast<double>(bg_image.get(pixel, channel))};
      const double variance{per_pixel_variance.at(pixel, channel)};
      // delta / variance can cause rounding issues to 0, hence brackets
      // required for safety with high variance values
      l2 += (delta * delta) / variance;
      det *= variance;
    }

    *probabilities.ptr<double>(pixel) =
        std::min<double>(pdf(l2, det) * magic_scaling, 1);
  }

  return Probability(std::move(probabilities));
}
