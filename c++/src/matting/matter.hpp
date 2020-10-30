#ifndef MATTERIALIZE_MATTER_HPP
#define MATTERIALIZE_MATTER_HPP

#include <opencv2/core.hpp>

class IMatter {
public:
  virtual cv::Mat background_mask(const cv::Mat &video_frame) = 0;

  cv::Mat change_background(const cv::Mat &video_frame, const cv::Mat &new_background);

  cv::Mat remove_background(const cv::Mat &video_frame);

  cv::Mat greenscreen(const cv::Mat &video_frame);
};

#endif
