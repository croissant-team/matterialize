#include "scorer.hpp"

ConfusionMatrix
Scorer::difference(Mat &predicted, Mat &truth, uchar threshold) {
  if (predicted.size != truth.size) {
    exit(-1);
  }

  ConfusionMatrix stats = ConfusionMatrix();
  for (int y = 0; y < predicted.rows; y++) {
    for (int x = 0; x < predicted.cols; x++) {
      int truth_index = truth.ptr<uchar>(y, x)[0] > threshold ? 0 : 1;
      int predicted_index = predicted.ptr<uchar>(y, x)[0] > threshold ? 0 : 1;
      stats.increment(truth_index, predicted_index);
    }
  }
  return stats;
}
