#ifndef MATTERIALIZE_GLOBAL_BG_MODEL_HPP
#define MATTERIALIZE_GLOBAL_BG_MODEL_HPP

#include "../types/image.hpp"
#include "gmm_global_color_model.hpp"
#include <opencv2/imgproc.hpp>
#include <opencv2/ml.hpp>

using namespace cv;
using namespace cv::ml;

class GlobalBgModel {
private:
  const Ptr<EM> gmm;

public:
  explicit GlobalBgModel(const Image &bg_image, int num_components);

  [[nodiscard]] Probability global_probs(const Image &img) const;
};

#endif// MATTERIALIZE_GLOBAL_BG_MODEL_HPP
