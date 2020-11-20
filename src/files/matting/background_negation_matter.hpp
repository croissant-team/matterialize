#ifndef MATTERIALIZE_BACKGROUND_NEGATION_MATTER_HPP
#define MATTERIALIZE_BACKGROUND_NEGATION_MATTER_HPP


#include <utility>

#include <opencv2/core.hpp>

#include "matter.hpp"
#include "matter_type.hpp"
class BackgroundNegationMatter : public IMatter {
private:
  const cv::Mat background;
  MatterConfig &config;

public:
  constexpr static MatterConfigField threshold =
      MatterConfigField{"threshold", "25", false};

  explicit BackgroundNegationMatter(
      cv::Mat background, MatterConfig &config) noexcept
      : background{std::move(background)}, config{config} {}

  cv::Mat background_mask(const cv::Mat &video_frame) override;
  bool requires_clean_plate() override;
};

// Example of how an implementer would create matter mode
class BackgroundNegationMode : IMatterMode {
  [[nodiscard]] string name() const override {
    return "Background Negation";
  }

  [[nodiscard]] vector<MatterConfigField> config_fields() const override {
    return {BackgroundNegationMatter::threshold};
  }

  [[nodiscard]] IMatter *
  init_matter(MatterInitData &data, MatterConfig &config) override {
    return new BackgroundNegationMatter(data.clean_plate.mat, config);
  }
};
#endif
