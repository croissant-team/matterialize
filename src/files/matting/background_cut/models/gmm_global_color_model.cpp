#include "gmm_global_color_model.hpp"

Probability
GMMGlobalColorModel::global_probs(const Ptr<EM> &trainedGMM, const Image &img) {
  Mat distribProbs{};
  Mat samples{img.to_samples()};
  trainedGMM->predict(samples, distribProbs);
  int ROW_SUM = 1;
  Mat weights{};
  // TODO might be able to cleanup with C++ overloaded operators but first
  // check for correcteness
  transpose(trainedGMM->getWeights(), weights);
  Mat probs{};
  gemm(distribProbs, weights, 1.0, Mat(), 0.0, probs, 0);
  reduce(probs, probs, ROW_SUM, REDUCE_SUM);

  return Probability(std::move(probs));
}
