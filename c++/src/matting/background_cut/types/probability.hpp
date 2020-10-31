#ifndef MATTERIALIZE_PROBABILITY_HPP
#define MATTERIALIZE_PROBABILITY_HPP

#include <utility>

#include <opencv2/core.hpp>

class Probability {
  const cv::Mat mat;

public:
  explicit Probability(cv::Mat &&t_mat) noexcept : mat{std::move(t_mat)} {}

  double get(int pixel_index);
};

#endif
