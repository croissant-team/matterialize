#include "converter.hpp"

using namespace std;

vector<double>
Converter::BGR_to_YUYV(vector<byte_t> bgr1, vector<byte_t> bgr2) {
  double y1 = (0.257 * bgr1[2]) + (0.504 * bgr1[1]) + (0.098 * bgr1[0]) + 16.0;
  double u = -(0.148 * bgr1[2]) - (0.291 * bgr1[1]) + (0.439 * bgr1[0]) + 128.0;
  double y2 = (0.257 * bgr2[2]) + (0.504 * bgr2[1]) + (0.098 * bgr2[0]) + 16.0;
  double v = (0.439 * bgr2[2]) - (0.368F * bgr2[1]) - (0.071 * bgr2[0]) + 128.0;
  double yuyv[] = {y1, u, y2, v};
  return vector<double>(yuyv, yuyv + 4);
}

vector<byte_t> Converter::to_YUYV(cv::Mat src) {
  vector<byte_t> output_data{};
  vector<byte_t> input_data{};
  src.copyTo(input_data);

  int rows = src.rows;
  int cols = src.cols;
  int inline_size = cols * 3;
  for (int i = 0; i < rows; i++) {
    int inoff = i * inline_size;
    for (int j = 0; j < cols; j += 2) {
      int index_src = inoff + j * 3;
      // TODO the conversion from byte_t (uint8_t) to double is a narrowing conversion (warned)
      byte_t bgr1[] = {
          input_data[index_src], input_data[index_src + 1],
          input_data[index_src + 2]};
      byte_t bgr2[] = {
          input_data[index_src + 3], input_data[index_src + 4],
          input_data[index_src + 5]};
      vector<double> yuyv = Converter::BGR_to_YUYV(
          vector<byte_t>(bgr1, bgr1 + 3), vector<byte_t>(bgr2, bgr2 + 3));

      output_data.insert(output_data.end(), yuyv.begin(), yuyv.end());
    }
  }

  return output_data;
}