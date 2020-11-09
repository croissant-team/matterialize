#ifndef MATTERIALIZE_COLOR_TERM_HPP
#define MATTERIALIZE_COLOR_TERM_HPP

#include <vector>
#include <opencv2/core.hpp>

using namespace cv;

using namespace std;

struct ColorTerm {
  const Mat bg_energies;
  const Mat fg_energies;

  explicit ColorTerm(Mat &&bg_energies, Mat &&fg_energies) noexcept
      : bg_energies{std::move(bg_energies)}, fg_energies{std::move(fg_energies)} {}
};

#endif//MATTERIALIZE_COLOR_TERM_HPP
