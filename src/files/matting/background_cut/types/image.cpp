#include "image.hpp"

#include <utility>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

int Image::num_channels() const {
  return mat.channels();
}

int Image::height() const {
  return mat.rows;
}

int Image::width() const {
  return mat.cols;
}

int Image::num_pixels() const {
  return mat.total();
}

uchar Image::get(int pixel_index, int channel) const {
  return mat.ptr<uchar>()[pixel_index * num_channels() + channel];
}

Image Image::resized(int rows, int cols) const {
  cv::Mat result{};

  cv::resize(
      mat, result,
      cv::Size2d(static_cast<double>(cols), static_cast<double>(rows)));

  return Image(std::move(result));
}

Image Image::downscaled(int factor) const {
  return resized(height() / factor, width() / factor);
}

Image Image::upscaled(int factor) const {
  return resized(height() * factor, width() * factor);
}

PixelVariance Image::get_pixel_variances() const {
  const cv::Size2d eight_neighborhood_size(3.0, 3.0);
  cv::Mat mat_of_doubles{};
  cv::Mat mean_of_square{};
  cv::Mat mean{};

  mat.convertTo(mat_of_doubles, CV_64FC3);

  cv::blur(
      mat_of_doubles.mul(mat_of_doubles), mean_of_square,
      eight_neighborhood_size);
  cv::blur(mat_of_doubles, mean, eight_neighborhood_size);

  const cv::Mat square_of_mean{mean.mul(mean)};
  cv::Mat variances{};

  cv::subtract(mean_of_square, square_of_mean, variances);

  return PixelVariance(std::move(variances));
}

cv::Mat Image::to_samples() const {
  cv::Mat result{};
  mat.clone().reshape(1, num_pixels()).convertTo(result, CV_32FC1);
  return result;
}
