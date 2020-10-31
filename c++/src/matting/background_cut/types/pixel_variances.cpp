#include "pixel_variances.hpp"

double PixelVariance::get(int pixel_index, int channel) {
  return data[pixel_index * num_channels + channel];
}
