#include "segmentation_result.hpp"
#include "image.hpp"
#include "mask.hpp"

#include <utility>

#include <opencv2/core.hpp>

SegmentationResult SegmentationResult::empty(int rows, int cols) {
  Image image(cv::Mat::zeros(rows, cols, CV_8UC3));
  Mask bg_mask(cv::Mat::zeros(rows, cols, CV_8UC1));

  return SegmentationResult(std::move(image), std::move(bg_mask));
}
