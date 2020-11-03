#ifndef MATTERIALIZE_WEBCAM_CONTROLS_HPP
#define MATTERIALIZE_WEBCAM_CONTROLS_HPP

#include <string>

using namespace std;

class WebcamControls {
private:
  const string device;

  string build_command(string attribute, string value);

public:
  void enable_automatic_controls();

  void disable_automatic_controls();

  explicit WebcamControls(int device_number)
      : device{"/dev/video" + to_string(device_number)} {
    enable_automatic_controls();
  }
};

#endif
