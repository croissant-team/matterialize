#include "color_model.hpp"
#include "../background_cut_matter.hpp"
#include <opencv2/core.hpp>

using namespace cv;
using namespace background_cut;

Probability ColorModel::mix_probs(const Image &img) const {
  Mat result{};
  double mix_factor = config.get(config_fields::color_model_mix_factor);
  if (mix_factor <= 0.0) {
    result = pixel_bg_model.get_pixel_probability(img).mat;
  } else if (mix_factor >= 1.0) {
    result = global_bg_model.global_probs(img).mat;
  } else {
    addWeighted(
        global_bg_model.global_probs(img).mat,
        mix_factor,
        pixel_bg_model.get_pixel_probability(img).mat,
        1 - mix_factor,
        0.0,
        result,
        CV_64FC1);
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
ColorModel::ColorModel(
    const Image &bg_image, MatterConfig &config,
    int global_fg_model_num_components, double global_fg_model_fg_threshold)
    : config{config},
      global_bg_model{
          bg_image,
          (int) config.get(config_fields::global_bg_model_num_components)},
      pixel_bg_model{bg_image},
      global_fg_model{
          global_bg_model,
          global_fg_model_num_components,
          global_fg_model_fg_threshold} {}
