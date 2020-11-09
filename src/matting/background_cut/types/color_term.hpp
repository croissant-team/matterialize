#ifndef MATTERIALIZE_COLOR_TERM_HPP
#define MATTERIALIZE_COLOR_TERM_HPP

#include <opencv2/core.hpp>
#include <vector>

using namespace cv;

using namespace std;

struct ColorTerm {
  const Mat bg_energies;
  const Mat fg_energies;

  explicit ColorTerm(Mat &&bg_energies, Mat &&fg_energies) noexcept
      : bg_energies{std::move(bg_energies)}, fg_energies{
                                                 std::move(fg_energies)} {}
};

#endif//MATTERIALIZE_COLOR_TERM_HPP
