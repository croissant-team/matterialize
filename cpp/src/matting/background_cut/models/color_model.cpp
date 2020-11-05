#include "color_model.hpp"
#include <opencv2/core.hpp>

using namespace cv;

Probability ColorModel::mix_probs(const Image &img) const {
  Mat result{};

  addWeighted(
      global_bg_model.global_probs(img).mat, mix_factor,
      pixel_bg_model.get_pixel_probability(img).mat, 1 - mix_factor, 0.0,
      result, CV_64FC1);

  return Probability(std::move(result));
}
ColorTerm ColorModel::color_term(const Image &img, const SegmentationResult &prev_segmentation_result) {
  Mat bgEnergies{};
  Mat fgEnergies{};

  log(mix_probs(img).mat, bgEnergies);
  multiply(bgEnergies, Scalar(-1.0), bgEnergies);

  log(global_fg_model.global_probs(img, prev_segmentation_result).mat, fgEnergies);
  multiply(fgEnergies, Scalar(-1.0), fgEnergies);

  return ColorTerm(std::move(bgEnergies), std::move(fgEnergies));
}
