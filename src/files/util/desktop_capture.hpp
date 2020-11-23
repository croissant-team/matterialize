#ifndef MATTERIALIZE_DESKTOP_CAPTURE_HPP
#define MATTERIALIZE_DESKTOP_CAPTURE_HPP

#include <string>
#include <vector>

class DesktopCapture {
public:
  DesktopCapture(std::string name, int x, int y, int width, int height);

  const std::string name;
  const int x;
  const int y;
  const int width;
  const int height;
};

class DesktopCaptures {
public:
  static std::vector<DesktopCapture> get_desktops();
  static std::string exec(const char* cmd);
};

#endif
