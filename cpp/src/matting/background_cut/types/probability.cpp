#include "probability.hpp"

double Probability::at(int pixel_index) const {
  return mat.ptr<double>()[pixel_index];
}
