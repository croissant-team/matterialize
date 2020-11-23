#include "background_negation_matter.hpp"

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

cv::Mat BackgroundNegationMatter::background_mask(const cv::Mat &video_frame) {
  cv::Mat mask(video_frame.size(), video_frame.type());

  cv::absdiff(video_frame, background, mask);
  cv::cvtColor(mask, mask, cv::COLOR_BGR2GRAY);

  double threshold{stod(config.get(threshold_field))};
  double max_val{255.0};

  cv::threshold(mask, mask, threshold, max_val, cv::THRESH_BINARY);

  return mask;
}

