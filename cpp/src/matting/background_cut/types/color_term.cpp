#include "color_term.hpp"

// Equation (6)
double ColorTerm::at(int pixel, mask::Label label) const {
  switch (label) {
    case mask::BACKGROUND:
      return *bg_energies.ptr<double>(pixel);
    case mask::FOREGROUND:
      return *fg_energies.ptr<double>(pixel);
  }
}
