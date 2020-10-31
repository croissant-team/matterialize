#ifndef MATTERIALIZE_IMAGE_HPP
#define MATTERIALIZE_IMAGE_HPP

#include "pixel_variances.hpp"

#include <utility>

#include <opencv2/core.hpp>

class Image {
private:
  const cv::Mat mat;

  int num_channels();

  int height();

  int width();

  int num_pixels();

public:
  explicit Image(cv::Mat &&t_mat) noexcept : mat{std::move(t_mat)} {}

  uchar get(int x, int y, int channel);

  PixelVariance get_pixel_variances();

  Image resized(int rows, int cols);

  Image downscaled(int factor);

  Image upscaled(int factor);
};

#endif
