#ifndef MATTERIALIZE_SEGMENTATION_RESULT_HPP
#define MATTERIALIZE_SEGMENTATION_RESULT_HPP

#include "image.hpp"
#include "mask.hpp"

#include <utility>

#include <opencv2/core.hpp>

struct SegmentationResult {
  const Image image;
  const Mask bg_mask;

  SegmentationResult(Image image, Mask bg_mask)
      : image{image}, bg_mask{bg_mask} {}

  static SegmentationResult empty(int rows, int cols);
};

#endif
