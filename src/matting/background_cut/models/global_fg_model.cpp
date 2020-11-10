#include "global_fg_model.hpp"
#include "gmm_global_color_model.hpp"

using namespace std;

Probability GlobalFgModel::global_probs(
    const Image &img, const SegmentationResult &prev_segmentation_result) {
  Mat samples{find_fg_samples(img, prev_segmentation_result)};
  // everything might be background (we have no data to train the foreground model)
  if (samples.empty()) {
    return Probability(std::move(Mat::zeros(img.mat.rows, img.mat.cols, CV_64FC1)));
  }
    gmm->trainEM(samples);
  return GMMGlobalColorModel::global_probs(gmm, img);
}
Mat GlobalFgModel::find_fg_samples(
    const Image &curr_img, const SegmentationResult &prev_segmentation_result) {
  const Probability probs{global_bg_model.global_probs(curr_img)};
  const Mask prev_bg_mask{prev_segmentation_result.bg_mask};
  const Image prev_img{prev_segmentation_result.image};

  const int num_channels{3};
  // max size is times 2 since we are taking samples from two images (current and previous)
  const int max_samples{curr_img.num_pixels() * 2};

  Mat samples(max_samples, num_channels, CV_32FC1);
  int num_samples{};

  for (int pixel{0}; pixel < curr_img.num_pixels(); pixel++) {
    double prob = probs.at(pixel);
    if (prob < fg_threshold) {
      // sample pixel from current image
      for (int channel{}; channel < num_channels; channel++) {
        *samples.ptr<float>(num_samples, channel) =
            curr_img.get(pixel, channel);
      }
      num_samples++;
    }

    // if pixel is in the intersection of F and the previous segmented foreground,
    // sample pixel from previous image to enforce temporal coherence
    if (prev_bg_mask.get(pixel) != 0) {
      for (int channel{}; channel < num_channels; channel++) {
        *samples.ptr<float>(num_samples, channel) =
            prev_img.get(pixel, channel);
      }
      num_samples++;
    }
  }

  // Truncate overallocated rows
  samples.resize(num_samples);

  return std::move(samples);
}

