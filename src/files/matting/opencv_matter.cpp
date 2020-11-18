#include "opencv_matter.hpp"

#include <opencv2/core.hpp>

cv::Mat OpenCVMatter::background_mask(const cv::Mat &video_frame) {
  cv::Mat mask{};

  background_subtractor->apply(video_frame, mask);

  return mask;
}

bool OpenCVMatter::requires_clean_plate() {
  return false;
}
