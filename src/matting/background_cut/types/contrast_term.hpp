#ifndef MATTERIALIZE_CONTRAST_TERM_HPP
#define MATTERIALIZE_CONTRAST_TERM_HPP

#include <utility>

#include "mask.hpp"

using namespace cv;

class ContrastTerm {
private:
  [[nodiscard]] int offset_in_contrasts(int p1, int p2) const;

public:
  int image_width;
  Mat contrasts;

  ContrastTerm(Mat contrasts, int image_width)
      : contrasts{std::move(contrasts)}, image_width{image_width} {};

  [[nodiscard]] double
  between(int pixel1, int pixel2, mask::Label label1, mask::Label label2) const;
};

#endif//MATTERIALIZE_CONTRAST_TERM_HPP
