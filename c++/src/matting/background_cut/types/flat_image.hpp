#ifndef MATTERIALIZE_FLAT_IMAGE_HPP
#define MATTERIALIZE_FLAT_IMAGE_HPP

#include <utility>

#include <opencv2/core.hpp>

class FlatImage {
private:
  const cv::Mat mat;
  const int num_channels;

public:
  explicit FlatImage(cv::Mat &&t_mat) noexcept
      : mat{std::move(t_mat)}, num_channels{3} {}

  [[nodiscard]] uchar get(int pixel_index, int channel) const;

  [[nodiscard]] int num_pixels() const;

  [[nodiscard]] cv::Mat to_samples() const;
};

#endif
