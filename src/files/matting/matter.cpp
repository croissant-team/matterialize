#include "matter.hpp"

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

cv::Mat IMatter::change_background(
    const cv::Mat &video_frame, const cv::Mat &new_background) {
  const cv::Mat &bg_mask{background_mask(video_frame)};
  cv::Mat fg_mask(bg_mask.size(), bg_mask.type());

  cv::bitwise_not(bg_mask, fg_mask);

  cv::Mat bg{};
  cv::Mat fg{};

  new_background.copyTo(bg, fg_mask);
  video_frame.copyTo(fg, bg_mask);

  cv::Mat result(fg.size(), fg.type());

  cv::add(fg, bg, result);

  return result;
}

cv::Mat IMatter::remove_background(const cv::Mat &video_frame) {
  const cv::Mat &fg_mask{background_mask(video_frame)};
  cv::Mat result{};

  video_frame.copyTo(result, fg_mask);

  return result;
}

cv::Mat IMatter::green_screen(const cv::Mat &video_frame) {
  const cv::Scalar scalar(0.0, 255.0, 0.0);
  const cv::Mat green_screen(video_frame.size(), video_frame.type(), scalar);

  return change_background(video_frame, green_screen);
}
