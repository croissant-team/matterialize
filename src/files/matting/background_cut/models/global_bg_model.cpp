#include "global_bg_model.hpp"
#include "gmm_global_color_model.hpp"

Probability GlobalBgModel::global_probs(const Image &img) const {
  return GMMGlobalColorModel::global_probs(gmm, img);
}

GlobalBgModel::GlobalBgModel(const Image &bg_image, int num_components)
    : gmm{EM::create()} {
  gmm->setClustersNumber(num_components);

  // Speeds up training though not necessary for training background model as
  // is only done once
  gmm->setCovarianceMatrixType(EM::COV_MAT_SPHERICAL);

  std::cout << "StartedTraining global background model"
            << "\n";
  gmm->trainEM(bg_image.to_samples());
  std::cout << "Finishing training global background model"
            << "\n";
}
