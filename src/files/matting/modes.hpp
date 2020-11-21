#ifndef MATTERIALIZE_MODES_H
#define MATTERIALIZE_MODES_H

#include "background_negation_matter.hpp"
#include "matter_mode.hpp"

// MatterModes enables the use of IMatterMode instances as glorified enums
class MatterModes {
private:
  constexpr static const BackgroundNegationMode BACKGROUND_NEGATION{};

public:
  constexpr static const IMatterMode *const modes[] = {&BACKGROUND_NEGATION};
  static const IMatterMode * get_by_name(string name) {
    for (auto mode : modes) {
      if (mode->name() == name) {
        return mode;
      }
    }

    throw invalid_argument("Invalid matter name");
  }
};

#endif// MATTERIALIZE_MODES_H
