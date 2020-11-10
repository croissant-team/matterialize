#include "compositor.hpp"

pair<Mat, Mat> Compositor::compose(Mat &background, Mat &foreground, Mat &mask) {
  if (foreground.size() == mask.size()) {
    ForegroundTransform transform = calculate_foreground_transform(background, foreground);
    resize(foreground, foreground, transform.size);
    resize(mask, mask, transform.size);
    Mat result_mask = Mat(background.size(), CV_8U, Scalar(0.0, 0.0, 0.0));
    Mat new_image;
    background.copyTo(new_image);
    for (int y = 0; y < foreground.rows; y++) {
      for (int x = 0; x < foreground.cols; x++) {
        double alpha = mask.ptr<double>(y, x)[0] / 255.0;
        int new_x = x + transform.x_offset;
        int new_y = y + transform.y_offset;
        vector<double> blend = lerp(background.ptr<double>(new_y, new_x), foreground.ptr<double>(new_y, new_x), alpha);
        for (int i = 0; i < 3; i++) {
          new_image.ptr<double>(new_y, new_x)[i] = blend[i];
        }
        result_mask.at<double>(new_y, new_x) = alpha * 255.0;
      }
    }
    return make_pair(new_image, result_mask);
  }
  return make_pair(background, mask);
}

Compositor::ForegroundTransform Compositor::calculate_foreground_transform(Mat &background, Mat &foreground) {
  double height = foreground.rows;
  double width = foreground.cols;
  if (height > background.rows) {
    double scale_factor = background.rows / height;
    height = background.rows;
    width *= scale_factor;
  }
  if (width > background.cols) {
    double scale_factor = background.cols / width;
    width = background.cols;
    height *= scale_factor;
  }
  ForegroundTransform result;
  result.x_offset = round((background.cols - width)) / 2;
  result.y_offset = round((background.rows - height));
  result.size = Size(width, height);
  return result;
}

vector<double> Compositor::lerp(
    double *background_values, double *foreground_values, double alpha) {
  vector<double> result;
  for (int i = 0; i < 3; i++) {
    if (alpha >= 1.0) {
      result.push_back(foreground_values[i]);
    } else if (alpha <= 0.0) {
      result.push_back(background_values[i]);
    } else {
      result.push_back((1.0 - alpha) * background_values[i] + alpha * foreground_values[i]);
    }
  }
  return result;
}
