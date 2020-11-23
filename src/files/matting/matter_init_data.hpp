#ifndef MATTERIALIZE_MATTER_INIT_DATA_HPP
#define MATTERIALIZE_MATTER_INIT_DATA_HPP

#include <opencv2/core/mat.hpp>

// The initialization data we give to the matter (this is the same for all
// matters) the matters can decide to ignore it or use it
struct MatterInitData {
  cv::Mat clean_plate;
};

#endif// MATTERIALIZE_MATTER_INIT_DATA_HPP
