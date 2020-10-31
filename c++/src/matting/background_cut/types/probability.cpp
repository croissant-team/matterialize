#include "probability.hpp"

double Probability::get(int pixel_index) {
  return mat.ptr<double>()[pixel_index];
}
