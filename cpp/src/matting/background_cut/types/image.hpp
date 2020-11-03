#ifndef MATTERIALIZE_IMAGE_HPP
#define MATTERIALIZE_IMAGE_HPP

#include "flat_image.hpp"
#include "pixel_variances.hpp"

#include <utility>

#include <opencv2/core.hpp>

class Image {
private:
  const cv::Mat mat;
  const FlatImage flat;

  [[nodiscard]] int num_channels() const;

  [[nodiscard]] int height() const;

  [[nodiscard]] int width() const;

  [[nodiscard]] int num_pixels() const;

public:
  explicit Image(cv::Mat &&t_mat) noexcept
      : mat{std::move(t_mat)}, flat{mat.clone().reshape(1, num_pixels())} {}

  [[nodiscard]] uchar get(int x, int y, int channel) const;

  [[nodiscard]] PixelVariance get_pixel_variances() const;

  [[nodiscard]] FlatImage flattened() const;

  [[nodiscard]] Image resized(int rows, int cols) const;

  [[nodiscard]] Image downscaled(int factor) const;

  [[nodiscard]] Image upscaled(int factor) const;
};

#endif
