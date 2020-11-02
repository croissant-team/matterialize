#ifndef MATTERIALIZE_CONVERTER_HPP
#define MATTERIALIZE_CONVERTER_HPP

#include <cstdint>
#include <opencv2/core.hpp>
#include <vector>

using namespace std;

typedef uint8_t byte_t;

class Converter {
private:
  static vector<double> BGR_to_YUYV(vector<byte_t> bgr1, vector<byte_t> bgr2);

public:
  static vector<byte_t> to_YUYV(cv::Mat src);
};

#endif
