#include "background_cut_matter.hpp"
#include <opencv2/imgproc.hpp>

#include "models/contrast_model.hpp"
#include "opencv2/imgcodecs.hpp"
#include "types/contrast_term.hpp"
#include "types/image.hpp"

cv::Mat BackgroundCutMatter::background_mask(const cv::Mat &video_frame) {
  Image img{cv::Mat(video_frame)};
  Image downscaled_img = img.downscaled(downscale_factor);
  Probability probs = color_model.mix_probs(downscaled_img);
  //ColorTerm color_term = color_model.color_term(downscaled_img, prev_segmentation_res);

  Mat downscaled_flat_mask{};
  threshold(probs.mat, downscaled_flat_mask, 0.05, 255.0, THRESH_BINARY_INV);
  downscaled_flat_mask.convertTo(downscaled_flat_mask, CV_8UC1);

  Mat downscaled_mask{downscaled_flat_mask.reshape(1, downscaled_img.mat.rows)};

  medianBlur(downscaled_mask, downscaled_mask, median_blur_kernel_size);
  // TODO call energy minimzation function
  Mat mask{};
  resize(
      downscaled_mask, mask, Size2d(video_frame.cols, video_frame.rows),
      INTER_LINEAR_EXACT);
  return std::move(mask);
}

bool BackgroundCutMatter::requires_clean_plate() {
  return true;
}
