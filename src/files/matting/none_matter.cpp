#include "none_matter.hpp"

cv::Mat NoneMatter::background_mask(const cv::Mat &video_frame) {
  return cv::Mat(video_frame.size(), CV_8UC1, cv::Scalar(255));
}

bool NoneMatter::requires_clean_plate() {
  return false;
}
