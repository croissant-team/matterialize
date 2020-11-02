#include "mask.hpp"

uchar Mask::get(int pixel_index) const {
  return mat.ptr<uchar>(0, 0)[pixel_index];
}

int Mask::num_pixels() const {
  return mat.size().width;
}
