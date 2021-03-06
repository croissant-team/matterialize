#include "pixel_variances.hpp"

double PixelVariance::at(int pixel_index, int channel) const {
  return mat.ptr<double>()[pixel_index * num_channels + channel];
}
