#include "contrast_term.hpp"

// Equation (7)
double ContrastTerm::at(
    int pixel1, int pixel2, mask::Label label1, mask::Label label2) const {
   double contrast = contrasts.ptr<double>()[pixel1 * 9 + offset_in_contrasts(pixel1, pixel2)];
   return abs(label1 - label2) * contrast;
}


// TODO Ask andreas if we need rows (i.e. height) for anything
int ContrastTerm::offset_in_contrasts(int p1, int p2) const {
  const int y1 = p1 / image_width;
  const int x1 = p1 % image_width;
  const int y2 = p2 / image_width;
  const int x2 = p2 % image_width;

  // clang-format off
  const int offsets[3][3] = {
      {0, 1, 2},
      {3, INT_MAX, 5},
      {6, 7, 8}
  };
  // clang-format on

  return offsets[1 + y2 - y1][1 + x2 - x1];
}
