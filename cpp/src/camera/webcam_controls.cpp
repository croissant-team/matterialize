#include "webcam_controls.hpp"

#include <cstdlib>
#include <iostream>

string WebcamControls::build_command(string attribute, string value) {
  return "v4l2-ctl -d " + device + " --set-ctrl=" + attribute + "=" + value;
}

void run(string command) {
  system(command.c_str());
}

void WebcamControls::enable_automatic_controls() {
  cout << "Enabling automatic webcam controls\n";
  run(build_command("exposure_auto", "3"));
  run(build_command("white_balance_temperature_auto", "1"));
}

void WebcamControls::disable_automatic_controls() {
  cout << "Disabling automatic webcam controls\n";
  run(build_command("exposure_auto", "1"));
  run(build_command("white_balance_temperature_auto", "0"));
}
