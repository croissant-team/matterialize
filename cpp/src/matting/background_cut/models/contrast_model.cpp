#include "contrast_model.hpp"
using namespace cv;

// TODO add ContrastTerm and ColorTerm get function to get the value for
// given pixel position and given labels
// Go over the code for the implementation with someone else to ensure correctness
ContrastTerm ContrastModel::contrast_term(const Image &img) const {
  const Mat img_mat = img.mat;

  const int neighborhood_size{9};
  const int neighborhood_cols{3};
  const int num_channels{3};

  Mat bordered_image{};
  const int border_size{1};
  copyMakeBorder(
      img_mat, bordered_image, border_size, border_size, border_size,
      border_size, BORDER_REPLICATE);


  Mat contrast{img.num_pixels() * neighborhood_size, CV_64FC1};

  for (int y{0}; y < img_mat.rows; ++y) {
    for (int x{0}; x < img_mat.cols; ++x) {
      const uchar *bgr1 = bordered_image.ptr<uchar>(x, y);

      for (int j{y - 1}; j < y + 1; ++j) {
        for (int i{x - 1}; i < x + 1; ++i) {

          const uchar *bgr2 = bordered_image.ptr<uchar>(x + i, y + j);
          double _contrast{0};

          for (int channel{0}; channel < num_channels; ++channel) {
            double diff = static_cast<double>(bgr1[channel]) - static_cast<double>(bgr2[channel]);
            _contrast += diff * diff;
          }

          int pixel_index{y * img_mat.cols + x};
          int contrast_index{pixel_index * neighborhood_size + j * neighborhood_cols + i};

          contrast.ptr<double>()[contrast_index] = _contrast;
        }
      }
    }
  }

  double contrast_sum = sum(contrast)[0];
  double expected_contrast = contrast_sum / contrast.total();

  // beta in equation (7) of the paper
  const double robust_parameter = 1 / (2 * expected_contrast);

  Mat contrast_term = contrast;
  exp((- robust_parameter) * contrast, contrast_term);

  // TODO not finished

  return ContrastTerm();
}
