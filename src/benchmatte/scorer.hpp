#ifndef MATTERIALIZE_SRC_BENCHMATTE_SCORER_HPP
#define MATTERIALIZE_SRC_BENCHMATTE_SCORER_HPP

#include "confusion_matrix.hpp"
#include <opencv2/opencv.hpp>

using namespace cv;

class Scorer {
public:
  static ConfusionMatrix
  difference(Mat &predicted, Mat &truth, uchar threshold = 127);
};

#endif
