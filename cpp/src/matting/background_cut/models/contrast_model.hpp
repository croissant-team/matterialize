#ifndef MATTERIALIZE_CONTRAST_MODEL_HPP
#define MATTERIALIZE_CONTRAST_MODEL_HPP

#include "../types/contrast_term.hpp"
#include "../types/image.hpp"

class ContrastModel {
  [[nodiscard]] ContrastTerm contrast_term(const Image &img) const;
};

#endif//MATTERIALIZE_CONTRAST_MODEL_HPP
