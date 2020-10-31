#ifndef MATTERIALIZE_SEGMENTATION_RESULT_HPP
#define MATTERIALIZE_SEGMENTATION_RESULT_HPP

#include "flat_mask.hpp"
#include "image.hpp"

#include <utility>

#include <opencv2/core.hpp>

class SegmentationResult {
private:
  const Image image;
  const FlatMask bg_mask;

  SegmentationResult(Image &&image, FlatMask &&bg_mask) noexcept
      : image{std::move(image)}, bg_mask{std::move(bg_mask)} {}

public:
  static SegmentationResult empty(int rows, int cols);
};

#endif
