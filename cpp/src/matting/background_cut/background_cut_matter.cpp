#include "background_cut_matter.hpp"

#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

#include "types/image.hpp"

cv::Mat BackgroundCutMatter::background_mask(const cv::Mat &video_frame) {
  Image img{cv::Mat(video_frame)};
  auto probs{color_bg_model.global_probs(img)};
  cv::Mat bg_gmm_flat_mask{};// F in the paper

  cv::threshold(
      probs.mat, bg_gmm_flat_mask, 0.05, 255.0, cv::THRESH_BINARY_INV);
  bg_gmm_flat_mask.convertTo(bg_gmm_flat_mask, CV_8UC1);

  auto reshaped = bg_gmm_flat_mask.reshape(1, video_frame.rows);
  cv::Mat result{};

  cv::medianBlur(reshaped, result, 21);
  cv::imwrite("/home/line/out.png", result);

  return reshaped;
}
