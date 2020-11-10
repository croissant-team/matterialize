#ifndef MATTERIALIZE_SRC_BENCHMATTE_COMPOSITOR_HPP
#define MATTERIALIZE_SRC_BENCHMATTE_COMPOSITOR_HPP

#include <opencv2/opencv.hpp>
#include <utility>

using namespace cv;
using namespace std;

class Compositor {
private:
  struct ForegroundTransform {
    int x_offset;
    int y_offset;
    Size size;
  };

  ForegroundTransform
  calculate_foreground_transform(Mat &background, Mat &foreground);
  vector<double>
  lerp(double *background_values, double *foreground_values, double alpha);

public:
  pair<Mat, Mat> compose(Mat &background, Mat &foreground, Mat &mask);
};

#endif
