#ifndef MATTERIALIZE_BACKGROUND_CUT_MATTER_HPP
#define MATTERIALIZE_BACKGROUND_CUT_MATTER_HPP

#include "../matter.hpp"
#include "models/pixel_bg_model.hpp"
#include "types/image.hpp"

#include <utility>
#include <iostream>

#include <opencv2/core.hpp>

class BackgroundCutMatter : public IMatter {
public:
  const PixelBGModel bgModel;

  explicit BackgroundCutMatter(const cv::Mat &&background) noexcept
      : bgModel{PixelBGModel(Image(cv::Mat(background)))} {
  }

  cv::Mat background_mask(const cv::Mat &video_frame) override;
};

#endif
