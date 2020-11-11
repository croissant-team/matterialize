#include "video_devices.hpp"

#include <cctype>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

std::vector<VideoDevice> VideoDevices::get_devices() {
  auto res =
      VideoDevices::exec("v4l2-ctl --list-devices | grep -A1 ' (.*):$' | sed "
                         "'s/ (.*)://' | sed '/\\-\\-/d' | sed 's/\t//'");

  std::string dev_num;
  std::string name;
  std::vector<VideoDevice> devices;
  bool parse_name{true};

  for (char c : res) {
    if (parse_name) {
      if (c != '\n') {
        name.push_back(c);
      } else {
        parse_name = false;
      }
    } else {
      if (c != '\n') {
        if (std::isdigit(c)) {
          dev_num.push_back(c);
        }
      } else {
        int num = stoi(dev_num);

        if (num != MATTERIALIZE_PREVIEW && num != MATTERIALIZE_CAM) {
          VideoDevice device(std::stoi(dev_num), name);
          devices.push_back(device);
        }

        dev_num.clear();
        name.clear();
        parse_name = true;
      }
    }
  }

  return devices;
}

std::string VideoDevices::exec(const char *cmd) {
  std::array<char, 128> buffer{};
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

VideoDevice::VideoDevice(int number, const std::string &name)
    : number(number), name(name) {}
