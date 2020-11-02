#ifndef MATTERIALIZE_SEGMENTATION_RESULT_HPP
#define MATTERIALIZE_SEGMENTATION_RESULT_HPP

#include "image.hpp"
#include "mask.hpp"

#include <utility>

#include <opencv2/core.hpp>

class SegmentationResult {
private:
  const Image image;
  const Mask bg_mask;

  SegmentationResult(Image &&image, Mask &&bg_mask) noexcept
      : image{std::move(image)}, bg_mask{std::move(bg_mask)} {}

public:
  static SegmentationResult empty(int rows, int cols);
};

#endif
