#include "pixel_bg_model.hpp"

#include <cmath>
#include <utility>

#include <opencv2/core.hpp>

constexpr auto PI{3.14159265358979323846};

double PixelBGModel::pdf(double l2, double det) const {
  return std::exp(-1 * l2 / 2) / std::sqrt(8 * PI * PI * PI * det);
}

Probability PixelBGModel::get_pixel_probability(const FlatImage &image) const {
  cv::Mat probabilities(
      cv::Size2d(1.0, static_cast<double>(image.num_pixels())), CV_64FC1);

  for (int pixel{}; pixel < image.num_pixels(); ++pixel) {
    double l2{};
    double det{};

    for (int channel{}; channel < 3; ++channel) {
      const double delta{
          static_cast<double>(image.get(pixel, channel)) -
          flat_bg_image.get(pixel, channel)};
      const double variance{per_pixel_variance.get(pixel, channel)};
      l2 += delta * delta / variance;
      det *= variance;
    }

    *probabilities.ptr<double>(pixel, 0) = pdf(l2, det);
  }

  return Probability(std::move(probabilities));
}
