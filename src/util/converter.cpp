#include "converter.hpp"

using namespace std;

// Inspired by the open source code found at http://jevois.org/doc/RawImageOps_8C_source.html#l01038
vector<uchar> Converter::to_YUYV(const cv::Mat &src) {
  vector<uchar> output_data(src.total() * 2);

  int rows = src.rows;
  int cols = src.cols;
  int inline_size = cols * 3;
  int outline_size = cols * 2;

  for (int y{0}; y < rows; ++y) {
    int input_offs = y * inline_size;
    int output_offs = y * outline_size;
    for (int x{0}; x < cols; x += 2) {
      int index_src = input_offs + x * 3;
      uchar bgr1[] = {
          src.data[index_src], src.data[index_src + 1],
          src.data[index_src + 2]};
      uchar bgr2[] = {
          src.data[index_src + 3], src.data[index_src + 4],
          src.data[index_src + 5]};

      double y1 = (0.257 * bgr1[2]) + (0.504 * bgr1[1]) + (0.098 * bgr1[0]) + 16.0;
      double u = -(0.148 * bgr1[2]) - (0.291 * bgr1[1]) + (0.439 * bgr1[0]) + 128.0;
      double y2 = (0.257 * bgr2[2]) + (0.504 * bgr2[1]) + (0.098 * bgr2[0]) + 16.0;
      double v = (0.439 * bgr2[2]) - (0.368F * bgr2[1]) - (0.071 * bgr2[0]) + 128.0;

      double yuyv[] = {y1, u, y2, v};
      for (int i{0}; i < 4; ++i) {
        output_data[output_offs + x * 2 + i] = yuyv[i];
      }
    }
  }

  return output_data;
}