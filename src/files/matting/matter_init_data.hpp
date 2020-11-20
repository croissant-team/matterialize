#ifndef MATTERIALIZE_MATTER_INIT_DATA_HPP
#define MATTERIALIZE_MATTER_INIT_DATA_HPP

#include <opencv2/core/mat.hpp>
class matter_init_data {};

class CleanPlate {
public:
  const cv::Mat &mat;

  explicit CleanPlate(cv::Mat &mat) : mat{mat} {};
};
// The initialization data we give to the matter (this is the same for all
// matters) the matters can decide to ignore it or use it
class MatterInitData {
public:
  CleanPlate clean_plate;
};
#endif// MATTERIALIZE_MATTER_INIT_DATA_HPP
