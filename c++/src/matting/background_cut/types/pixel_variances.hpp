#ifndef MATTERIALIZE_PIXEL_VARIANCES_HPP
#define MATTERIALIZE_PIXEL_VARIANCES_HPP

#include <utility>

#include <opencv2/core.hpp>

class PixelVariance {
private:
  const cv::Mat mat;
  const int num_channels;

public:
  explicit PixelVariance(cv::Mat &&t_mat) noexcept
      : mat{std::move(t_mat)}, num_channels{mat.channels()} {}

  [[nodiscard]] double get(int pixel_index, int channel) const;
};

#endif
