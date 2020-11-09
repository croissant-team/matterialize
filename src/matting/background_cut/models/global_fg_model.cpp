#include "global_fg_model.hpp"
#include "gmm_global_color_model.hpp"

using namespace std;

Probability GlobalFgModel::global_probs(
    const Image &img, const SegmentationResult &prev_segmentation_result) {
  Mat samples{find_fg_samples(img, prev_segmentation_result)};
  gmm->trainEM(samples);
  return GMMGlobalColorModel::global_probs(gmm, img);
}
Mat GlobalFgModel::find_fg_samples(
    const Image &curr_img, const SegmentationResult &prev_segmentation_result) {
  const Probability probs{global_bg_model.global_probs(curr_img)};
  const Mask prev_bg_mask{prev_segmentation_result.bg_mask};
  const int num_channels = 3;
  const Image prev_img{prev_segmentation_result.image};

  // max size is times 2 since we are taking samples from two images (current and previous)
  int max_samples = curr_img.num_pixels() * num_channels * 2;

  Mat samples(max_samples, num_channels, CV_32FC1);
  int num_samples{};

  for (int pixel{}; pixel < curr_img.num_pixels(); pixel++) {
    if (probs.at(pixel) < fg_threshold) {
      // sample pixel from current image
      for (int channel{}; channel < num_channels; channel++) {
        *samples.ptr<uchar>(num_samples, channel) =
            curr_img.get(pixel, channel);
      }
      num_samples++;
    }

    // if pixel is in the intersection of F and the previous segmented foreground,
    // sample pixel from previous image to enforce temporal coherence
    if (prev_bg_mask.get(pixel) != 0) {
      for (int channel{}; channel < num_channels; channel++) {
        *samples.ptr<uchar>(num_samples, channel) =
            prev_img.get(pixel, channel);
      }
      num_samples++;
    }
  }

  samples.resize(num_samples);

  return samples;
}
GlobalFgModel::GlobalFgModel(const GlobalBgModel &global_bg_model)
    : gmm{EM::create()}, fg_threshold{1E-20}, num_components{5},
      global_bg_model{global_bg_model} {
  TermCriteria term_criteria = gmm->getTermCriteria();
  gmm->setClustersNumber(num_components);

  // assumes independence of color channels (speeds up training)
  gmm->setCovarianceMatrixType(EM::COV_MAT_SPHERICAL);
}
