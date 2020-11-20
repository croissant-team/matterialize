#include "background_settings.hpp"

#include <X11/Xlib.h>
#include <X11/Xutil.h>

BackgroundSettings::BackgroundSettings(int width, int height)
    : mode{BackgroundMode::CLEAR}, width{width}, height{height} {
  const cv::Scalar scalar(0.0, 255.0, 0.0);
  green_screen = cv::Mat(height, width, CV_8UC3, scalar);

  bg = green_screen.clone();

  blur_size = 25;
}

cv::Mat BackgroundSettings::generate_background(cv::Mat frame) {
  switch (mode) {
    case BackgroundMode::STATIC: {
      return bg;
    }
    case BackgroundMode::DESKTOP: {
      Display* display = XOpenDisplay(nullptr);
      Window root = DefaultRootWindow(display);

      XWindowAttributes attributes = {0};
      XGetWindowAttributes(display, root, &attributes);

      XImage* img = XGetImage(display, root, desktop_x, desktop_y, desktop_w, desktop_h, AllPlanes, ZPixmap);
      int bpp = img->bits_per_pixel;

      std::vector<std::uint8_t> pix{};

      pix.resize(desktop_w * desktop_h * 4);

      memcpy(&pix[0], img->data, pix.size());

      XDestroyImage(img);
      XCloseDisplay(display);

      cv::Mat raw{desktop_h, desktop_w, bpp > 24 ? CV_8UC4 : CV_8UC3, &pix[0]};

      if (raw.type() == CV_8UC4) {
        cv::cvtColor(raw, raw, cv::COLOR_BGRA2BGR);
      }

      cv::Mat result{};

      cv::resize(raw, result, get_size());
      return result;
    }
    case BackgroundMode::BLUR: {
      cv::Mat blurred{};
      blur(frame, blurred, cv::Size(blur_size, blur_size));

      return blurred;
    }
    case BackgroundMode::CLEAR: {
      return green_screen;
    }
  }
}

void BackgroundSettings::set_background(cv::Mat background) {
  bg = std::move(background);
}

void BackgroundSettings::set_blur_size(int b_size) {
  blur_size = b_size;
}

cv::Size BackgroundSettings::get_size() {
  return cv::Size(width, height);
}

void BackgroundSettings::set_desktop(int x, int y, int w, int h) {
  desktop_x = x;
  desktop_y = y;
  desktop_w = w;
  desktop_h = h;
}
