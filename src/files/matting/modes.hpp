#ifndef MATTERIALIZE_MODES_H
#define MATTERIALIZE_MODES_H

#include "background_negation_matter.hpp"
#include "background_cut/background_cut_matter.hpp"
#include "opencv_matter.hpp"
#include "none_matter.hpp"
#include "matter_mode.hpp"

using MatterMode = const IMatterMode *;

// MatterModes enables the use of IMatterMode instances as glorified enums
class MatterModes {
private:
  constexpr static const BackgroundNegationMode BACKGROUND_NEGATION{};
  constexpr static const OpenCVMode OPENCV{};
  constexpr static const BackgroundCutMode  BACKGROUND_CUT{};

public:
  constexpr static const NoneMode NONE{};
  constexpr static MatterMode const modes[] = {
      &NONE, &BACKGROUND_NEGATION, &OPENCV, &BACKGROUND_CUT};
  constexpr static int num_modes = sizeof(modes) / sizeof(MatterMode);

  static MatterMode get_by_name(string name) {
    for (auto mode : modes) {
      if (mode->name() == name) {
        return mode;
      }
    }

    throw invalid_argument("Invalid matter name");
  }
};

#endif// MATTERIALIZE_MODES_H
