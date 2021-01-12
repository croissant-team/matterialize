#include "background_cut_matter.hpp"
#include <opencv2/imgproc.hpp>

#include "models/contrast_model.hpp"
#include "opencv2/imgcodecs.hpp"
#include "types/contrast_term.hpp"
#include "types/image.hpp"

using namespace background_cut;

cv::Mat BackgroundCutMatter::background_mask(const cv::Mat &video_frame) {
  Image img{cv::Mat(video_frame)};
  Image downscaled_img = img.downscaled(downscale_factor);
  Probability probs = color_model.mix_probs(downscaled_img);

  Mat downscaled_flat_mask{};
  threshold(probs.mat, downscaled_flat_mask, 0.05, 255.0, THRESH_BINARY_INV);
  downscaled_flat_mask.convertTo(downscaled_flat_mask, CV_8UC1);

  Mat downscaled_mask{downscaled_flat_mask.reshape(1, downscaled_img.mat.rows)};

  int median_blur_kernel_size_value =
      (int) config.get(config_fields::median_blur_kernel_size);
  medianBlur(downscaled_mask, downscaled_mask, median_blur_kernel_size_value);

  // TODO call energy minimzation function
  Mat mask{};
  resize(
      downscaled_mask,
      mask,
      Size2d(video_frame.cols, video_frame.rows),
      INTER_LINEAR_EXACT);
  return std::move(mask);
}
BackgroundCutMatter::BackgroundCutMatter(
    const Mat &background, MatterConfig &config, int downscale_factor,
    int global_fg_model_num_components,
    double global_fg_model_fg_threshold) noexcept
    : config{config},
      downscale_factor{downscale_factor},
      prev_segmentation_res{SegmentationResult::empty(
          background.rows / downscale_factor,
          background.cols / downscale_factor)},
      color_model{
          Image(std::move(background)).downscaled(downscale_factor),
          config,
          global_fg_model_num_components,
          global_fg_model_fg_threshold} {}
