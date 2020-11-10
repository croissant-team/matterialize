#include "scorer.hpp"

ConfusionMatrix
Scorer::difference(Mat &predicted, Mat &truth, double threshold) {
  if (predicted.size != truth.size) {
    exit(-1);
  }

  ConfusionMatrix stats = ConfusionMatrix();
  for (int i = 0; i < predicted.rows; i++) {
    for (int j = 0; j < predicted.cols; j++) {
      int truth_index = truth.ptr<double>(i, j)[0] > threshold ? 1 : 0;
      int predicted_index = predicted.ptr<double>(i, j)[0] > threshold ? 1 : 0;
      stats.increment(truth_index, predicted_index);
    }
  }
  return stats;
}
