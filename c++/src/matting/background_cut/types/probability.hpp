#ifndef MATTERIALIZE_PROBABILITY_HPP
#define MATTERIALIZE_PROBABILITY_HPP

#include <utility>

#include <opencv2/core.hpp>

class Probability {
  const cv::Mat mat;
  const double *data;

public:
  explicit Probability(cv::Mat &&t_mat) noexcept
      : mat{t_mat}, data{mat.ptr<double>()} {}

  double get(int pixel_index);
};

#endif
