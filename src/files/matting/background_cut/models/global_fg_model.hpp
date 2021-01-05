#ifndef MATTERIALIZE_GLOBAL_FG_MODEL_HPP
#define MATTERIALIZE_GLOBAL_FG_MODEL_HPP

#include "../types/image.hpp"
#include "../types/probability.hpp"
#include "../types/segmentation_result.hpp"
#include "global_bg_model.hpp"
#include <opencv2/ml.hpp>

using namespace cv::ml;
using namespace cv;

class GlobalFgModel {
private:
  const Ptr<EM> gmm;
  const GlobalBgModel global_bg_model;
  const double fg_threshold;

public:
  // TODO Check with Devesh that the global_bg_model can be passed here without
  // memory issues
  GlobalFgModel(
      const GlobalBgModel &global_bg_model, int num_components,
      double fg_threshold)
      : gmm{EM::create()},
        global_bg_model{global_bg_model},
        fg_threshold{fg_threshold} {
    gmm->setClustersNumber(num_components);
    // assumes independence of color channels (speeds up training)
    gmm->setCovarianceMatrixType(EM::COV_MAT_SPHERICAL);
  }

  [[nodiscard]] Probability global_probs(
      const Image &img, const SegmentationResult &prev_segmentation_result);
  Mat find_fg_samples(
      const Image &curr_img,
      const SegmentationResult &prev_segmentation_result);
};

#endif// MATTERIALIZE_GLOBAL_FG_MODEL_HPP
