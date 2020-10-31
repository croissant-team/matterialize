#include "flat_image.hpp"

#include <opencv2/core.hpp>

uchar FlatImage::get(int pixel_index, int channel) {
  return mat.ptr<uchar>(0, 0)[pixel_index * num_channels + channel];
}

int FlatImage::num_pixels() {
  return mat.size().height;
}

cv::Mat FlatImage::to_samples() {
  cv::Mat result{};

  mat.reshape(1, num_pixels()).convertTo(result, CV_32FC1);

  return result;
}
