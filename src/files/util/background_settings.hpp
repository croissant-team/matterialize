#ifndef MATTERIALIZE_BACKGROUND_SETTINGS_HPP
#define MATTERIALIZE_BACKGROUND_SETTINGS_HPP

#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>

enum class BackgroundMode { STATIC, BLUR, DESKTOP, CLEAR };

class BackgroundSettings {
public:
  BackgroundMode mode;

  BackgroundSettings(int width, int height);
  cv::Mat generate_background(cv::Mat frame);
  cv::Size get_size();

  void set_background(cv::Mat background);
  void set_blur_size(int b_size);
  void set_desktop(int x, int y, int w, int h);

private:
  cv::Mat bg;
  cv::Mat green_screen;
  int width;
  int height;
  int blur_size;

  int desktop_x;
  int desktop_y;
  int desktop_w;
  int desktop_h;
};

#endif
