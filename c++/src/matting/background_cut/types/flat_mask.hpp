#ifndef MATTERIALIZE_FLAT_MASK_HPP
#define MATTERIALIZE_FLAT_MASK_HPP

#include <opencv2/core.hpp>

class FlatMask {
private:
  const cv::Mat mat;
  const int num_channels;
  const uchar *data;

public:
  explicit FlatMask(cv::Mat &&t_mat) noexcept
      : mat{std::move(t_mat)}, num_channels{1}, data{mat.ptr<uchar>(0, 0)} {}

  uchar get(int pixel_index);

  int num_pixels();
};

#endif
