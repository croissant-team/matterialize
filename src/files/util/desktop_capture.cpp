#include "desktop_capture.hpp"

#include <iostream>
#include <memory>
#include <regex>

DesktopCapture::DesktopCapture(
    std::string name, int x, int y, int width, int height)
    : name{name}, x{x}, y{y}, width{width}, height{height} {}

std::vector<DesktopCapture> DesktopCaptures::get_desktops() {
  auto res = DesktopCaptures::exec("xrandr | grep ' connected'");

  std::regex re{"(.*?) connected.*? ([0-9]+)x([0-9]+)\\+([0-9]+)\\+([0-9]+)"};

  std::vector<DesktopCapture> desktops;

  std::sregex_iterator i{res.begin(), res.end(), re};
  std::sregex_iterator end{};

  while (i != end) {
    std::smatch match = *i;

    std::string name = match[1];
    int x = std::stoi(match[4]);
    int y = std::stoi(match[5]);
    int width = std::stoi(match[2]);
    int height = std::stoi(match[3]);

    DesktopCapture desktop{name, x, y, width, height};

    desktops.push_back(desktop);

    ++i;
  }

  return desktops;
}

std::string DesktopCaptures::exec(const char *cmd) {
  std::array<char, 1024> buffer{};
  std::string result;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
  if (!pipe) {
    throw std::runtime_error("popen() failed!");
  }
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    result += buffer.data();
  }
  return result;
}
