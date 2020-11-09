#ifndef MATTERIALIZE_MASK_HPP
#define MATTERIALIZE_MASK_HPP

#include <utility>

#include <opencv2/core.hpp>

class Mask {
private:
  const cv::Mat mat;
  const int num_channels;

public:
  explicit Mask(cv::Mat &&t_mat) noexcept
      : mat{std::move(t_mat)}, num_channels{1} {}

  [[nodiscard]] uchar get(int pixel_index) const;

  [[nodiscard]] int num_pixels() const;
};

#endif
