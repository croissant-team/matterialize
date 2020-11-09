#ifndef MATTERIALIZE_GMM_GLOBAL_COLOR_MODEL_HPP
#define MATTERIALIZE_GMM_GLOBAL_COLOR_MODEL_HPP

#include "../types/image.hpp"
#include "../types/probability.hpp"

#include <opencv2/ml.hpp>

using namespace cv::ml;
using namespace cv;

namespace GMMGlobalColorModel {
  [[nodiscard]] Probability global_probs(const Ptr<EM>& trainedGMM, const Image &img);
}

#endif//MATTERIALIZE_GMM_GLOBAL_COLOR_MODEL_HPP
