#include "probability.hpp"

double Probability::get(int pixel_index) const {
  return mat.ptr<double>()[pixel_index];
}
