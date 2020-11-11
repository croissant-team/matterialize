#ifndef MATTERIALIZE_PROBABILITY_HPP
#define MATTERIALIZE_PROBABILITY_HPP

#include <utility>

#include <opencv2/core.hpp>

class Probability {
public:
  const cv::Mat mat;
  explicit Probability(cv::Mat &&t_mat) noexcept : mat{std::move(t_mat)} {}

  [[nodiscard]] double at(int pixel_index) const;
};

#endif
