#ifndef MATTERIALIZE_PIXEL_VARIANCES_HPP
#define MATTERIALIZE_PIXEL_VARIANCES_HPP

#include <opencv2/core.hpp>

class PixelVariance {
private:
  const cv::Mat mat;
  const int num_channels;
  const double *data;

public:
  explicit PixelVariance(cv::Mat &&t_mat)
      : mat{t_mat}, num_channels{mat.channels()},
        data{reinterpret_cast<double *>(mat.data)} {}

  double get(int pixel_index, int channel);
};

#endif
