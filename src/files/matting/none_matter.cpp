#include "none_matter.hpp"

cv::Mat NoneMatter::background_mask(const cv::Mat &video_frame) {
  return cv::Mat(video_frame.size(), CV_8UC1, cv::Scalar(255));
}

const string NoneMode::name() const {
  return "None";
}
IMatter *
NoneMode::init_matter(MatterInitData &data, MatterConfig &config) const {
  return new NoneMatter();
}
vector<MatterConfigField> NoneMode::config_fields() const {
  return vector<MatterConfigField>();
}
bool NoneMode::requires_clean_plate() const {
  return false;
}
