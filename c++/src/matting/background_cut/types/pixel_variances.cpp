#include "pixel_variances.hpp"

double PixelVariance::get(int pixel_index, int channel) {
  return mat.ptr<double>()[pixel_index * num_channels + channel];
}
