#ifndef MATTERIALIZE_FLAT_IMAGE_HPP
#define MATTERIALIZE_FLAT_IMAGE_HPP

#include <utility>

#include <opencv2/core.hpp>

class FlatImage {
private:
  const cv::Mat mat;
  const int num_channels;
  const uchar *data;

public:
  explicit FlatImage(cv::Mat &&t_mat)
      : mat{std::move(t_mat)}, num_channels{3}, data{mat.ptr<uchar>(0, 0)} {}

  uchar get(int pixel_index, int channel);

  int num_pixels();

  cv::Mat to_samples();
};

#endif
