#include "color_model.hpp"
#include <opencv2/core.hpp>

using namespace cv;

Probability ColorModel::mix_probs(const Image &img) const {
  Mat result{};

  if (mix_factor <= 0.0) {
    result = pixel_bg_model.get_pixel_probability(img).mat;
  } else if (mix_factor >= 1.0) {
    result = global_bg_model.global_probs(img).mat;
  } else {
    addWeighted(
        global_bg_model.global_probs(img).mat, mix_factor,
        pixel_bg_model.get_pixel_probability(img).mat, 1 - mix_factor, 0.0,
        result, CV_64FC1);
  }

  return Probability(std::move(result));
}
ColorTerm ColorModel::color_term(
    const Image &img, const SegmentationResult &prev_segmentation_result) {
  Mat bg_energies{};
  Mat fg_energies{};

  log(mix_probs(img).mat, bg_energies);
  multiply(bg_energies, Scalar(-1.0), bg_energies);

  log(global_fg_model.global_probs(img, prev_segmentation_result).mat,
      fg_energies);
  multiply(fg_energies, Scalar(-1.0), fg_energies);

  return ColorTerm(std::move(bg_energies), std::move(fg_energies));
}
