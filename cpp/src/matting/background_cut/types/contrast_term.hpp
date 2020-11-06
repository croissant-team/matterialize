#ifndef MATTERIALIZE_CONTRAST_TERM_HPP
#define MATTERIALIZE_CONTRAST_TERM_HPP

#include "mask.hpp"
class ContrastTerm {
public:
  [[nodiscard]] double
  at(int pixel1, mask::Label label1, int pixel2, mask::Label label2) const;
};

#endif//MATTERIALIZE_CONTRAST_TERM_HPP
