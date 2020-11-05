#ifndef MATTERIALIZE_CONVERTER_HPP
#define MATTERIALIZE_CONVERTER_HPP

#include <cstdint>
#include <opencv2/core.hpp>
#include <vector>

using namespace std;

class Converter {
public:
  static vector<uchar> to_YUYV(const cv::Mat &src);
};

#endif
