#include "flat_mask.hpp"

uchar FlatMask::get(int pixel_index) {
  return data[pixel_index];
}

int FlatMask::num_pixels() {
  return mat.size().width;
}
