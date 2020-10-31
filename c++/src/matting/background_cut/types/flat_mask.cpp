#include "flat_mask.hpp"

uchar FlatMask::get(int pixel_index) {
  return mat.ptr<uchar>(0, 0)[pixel_index];
}

int FlatMask::num_pixels() {
  return mat.size().width;
}
