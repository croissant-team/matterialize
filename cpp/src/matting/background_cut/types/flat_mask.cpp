#include "flat_mask.hpp"

uchar FlatMask::get(int pixel_index) const {
  return mat.ptr<uchar>(0, 0)[pixel_index];
}

int FlatMask::num_pixels() const {
  return mat.size().width;
}
