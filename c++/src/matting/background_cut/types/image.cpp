#include "image.hpp"

#include <utility>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

int Image::num_channels() {
  return mat.channels();
}

int Image::height() {
  return mat.size().height;
}

int Image::width() {
  return mat.size().width;
}

int Image::num_pixels() {
  return mat.total();
}

uchar Image::get(int x, int y, int channel) {
  return mat.ptr<uchar>()[(y * width() + x) * num_channels() + channel];
}

PixelVariance Image::get_pixel_variances() {
  const cv::Size2d eight_neighborhood_size(3.0, 3.0);
  const cv::Mat mat_of_doubles{};
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

  cv::Mat flat_variances{variances.reshape(3, num_pixels())};

  return PixelVariance(std::move(flat_variances));
}

Image Image::resized(int rows, int cols) {
  cv::Mat result{};
  const cv::Size2d size(static_cast<double>(rows), static_cast<double>(cols));

  cv::resize(mat, result, size);

  return Image(std::move(result));
}

Image Image::downscaled(int factor) {
  return resized(width() / factor, height() / factor);
}

Image Image::upscaled(int factor) {
  return resized(width() * factor, height() * factor);
}
