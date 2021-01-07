#ifndef MATTERIALIZE_BACKGROUND_NEGATION_MATTER_HPP
#define MATTERIALIZE_BACKGROUND_NEGATION_MATTER_HPP

#include <utility>

#include <opencv2/core.hpp>

#include "matter.hpp"
#include "matter_config.hpp"
#include "matter_mode.hpp"

const static MatterConfigFieldDouble threshold_field = {
    "Threshold (lower: more background)", "threshold", false, 25.0, 0.0, 255.0, 0.1};

class BackgroundNegationMatter : public IMatter {
private:
  const cv::Mat background;
  MatterConfig &config;

public:
  // constexpr static MatterConfigDoubleField threshold_field =
  //    MatterConfigDoubleField{{"threshold", false}, 25.0, 0.0, 255.0, 0.1};

  explicit BackgroundNegationMatter(
      cv::Mat background, MatterConfig &config) noexcept
      : background{std::move(background)}, config{config} {}

  cv::Mat background_mask(const cv::Mat &video_frame) override;
};

// Example of how an implementer would create matter mode
class BackgroundNegationMode : public IMatterMode {
public:
  [[nodiscard]] const string name() const override {
    return "Background Negation";
  }

  [[nodiscard]] vector<const IMatterConfigField *>
  config_fields() const override {
    return {&threshold_field};
  }

  [[nodiscard]] bool requires_clean_plate() const override {
    return true;
  }

  [[nodiscard]] IMatter *
  init_matter(MatterInitData &data, MatterConfig &config) const override {
    return new BackgroundNegationMatter(data.clean_plate, config);
  }
};
#endif
