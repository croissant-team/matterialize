#ifndef MATTERIALIZE_COLOR_TERM_HPP
#define MATTERIALIZE_COLOR_TERM_HPP

#include <opencv2/core.hpp>
#include <vector>

#include "mask.hpp"

using namespace cv;

using namespace std;

class ColorTerm {
private:
  const Mat bg_energies;
  const Mat fg_energies;

public:
  explicit ColorTerm(Mat &&bg_energies, Mat &&fg_energies) noexcept
      : bg_energies{std::move(bg_energies)}, fg_energies{
                                                 std::move(fg_energies)} {}

  [[nodiscard]] double at(int pixel, mask::Label label) const;
};

#endif//MATTERIALIZE_COLOR_TERM_HPP
