#ifndef MATTERIALIZE_IMAGE_HPP
#define MATTERIALIZE_IMAGE_HPP

#include "pixel_variances.hpp"

#include <utility>

#include <iostream>
#include <opencv2/core.hpp>

class Image {
private:
  const cv::Mat mat;

  [[nodiscard]] int num_channels() const;

  [[nodiscard]] int height() const;

  [[nodiscard]] int width() const;

public:
  explicit Image(const cv::Mat &&t_mat) noexcept
      : mat{std::move(t_mat)} {}

  //[[nodiscard]] uchar get(int x, int y, int channel) const;
  [[nodiscard]] uchar get(int pixel_index, int channel) const;

  [[nodiscard]] PixelVariance get_pixel_variances() const;

  [[nodiscard]] int num_pixels() const;

  [[nodiscard]] cv::Mat to_samples() const;
};

#endif
