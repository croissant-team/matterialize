#ifndef MATTERIALIZE_NONE_MATTER_HPP
#define MATTERIALIZE_NONE_MATTER_HPP

#include "matter.hpp"

class NoneMatter : public IMatter {
public:
  cv::Mat background_mask(const cv::Mat &video_frame) override;
  bool requires_clean_plate() override;
};

#endif
