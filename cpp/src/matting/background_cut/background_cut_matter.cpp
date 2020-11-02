#include "background_cut_matter.hpp"

#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include <utility>

#include "types/image.hpp"
#include <iostream>

cv::Mat BackgroundCutMatter::background_mask(const cv::Mat &video_frame) {
  Image img{cv::Mat(video_frame)};
  auto probs{bg_model.get_pixel_probability(img)};

  cv::Mat bg_gmm_flat_mask{};// F in the paper

  cv::threshold(probs.mat, bg_gmm_flat_mask, 1E-15, 255.0, 1);
  bg_gmm_flat_mask.convertTo(bg_gmm_flat_mask, CV_8UC(1));

  auto reshaped = bg_gmm_flat_mask.reshape(1, video_frame.rows);
  cv::Mat result{};

  cv::medianBlur(reshaped, result, 21);
  cv::imwrite("/home/line/out.png", result);

  return result;
}
