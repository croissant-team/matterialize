#ifndef MATTERIALIZE_MATTER_HPP
#define MATTERIALIZE_MATTER_HPP

#include <opencv2/core.hpp>

enum class MatterMode { BACKGROUND_NEGATION, OPENCV, BACKGROUND_CUT };

class IMatter {
public:
  virtual ~IMatter() = default;

  virtual cv::Mat background_mask(const cv::Mat &video_frame) = 0;

  cv::Mat
  change_background(const cv::Mat &video_frame, const cv::Mat &new_background);

  cv::Mat remove_background(const cv::Mat &video_frame);

  cv::Mat green_screen(const cv::Mat &video_frame);
};

#endif
