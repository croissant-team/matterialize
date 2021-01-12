#ifndef MATTERIALIZE_NONE_MATTER_HPP
#define MATTERIALIZE_NONE_MATTER_HPP

#include "matter.hpp"
#include "matter_mode.hpp"

class NoneMatter : public IMatter {
public:
  cv::Mat background_mask(const cv::Mat &video_frame) override;
};

class NoneMode : public IMatterMode {
public:
  [[nodiscard]] const string name() const {
    return "None";
  }

  IMatter *
  init_matter(MatterInitData &data, MatterConfig &config) const override {
    return new NoneMatter();
  }

  [[nodiscard]] vector<const IMatterConfigField *>
  config_fields() const override {
    return {};
  }

  [[nodiscard]] bool requires_clean_plate() const override {
    return false;
  }
};

#endif
