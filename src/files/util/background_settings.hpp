#ifndef MATTERIALIZE_BACKGROUND_SETTINGS_HPP
#define MATTERIALIZE_BACKGROUND_SETTINGS_HPP

#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

enum class BackgroundMode { STATIC, BLUR, DESKTOP, CLEAR, VIDEO };

class BackgroundSettings {
public:
  BackgroundMode mode;

  BackgroundSettings(int width, int height);
  cv::Mat generate_background(cv::Mat frame);
  cv::Size get_size();

  void set_background(cv::Mat background);
  void set_blur_size(int b_size);
  void set_desktop(int x, int y, int w, int h);
  void set_video(cv::VideoCapture capture);

private:
  cv::Mat bg;
  cv::Mat green_screen;
  cv::VideoCapture cap;
  int cap_frame_count{-1};
  int cap_frame_current{-1};

  int width;
  int height;
  int blur_size;

  int desktop_x{-1};
  int desktop_y{-1};
  int desktop_w{-1};
  int desktop_h{-1};
};

#endif
