#include "global_bg_model.hpp"
#include "gmm_global_color_model.hpp"

Probability GlobalBgModel::global_probs(const Image &img) const {
  return GMMGlobalColorModel::global_probs(gmm, img);
}
