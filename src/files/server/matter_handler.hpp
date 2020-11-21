#ifndef MATTERIALIZE_MATTER_HANDLER_HPP
#define MATTERIALIZE_MATTER_HANDLER_HPP

#include "../camera/opencv_webcam.hpp"
#include "../camera/opencv_webcam_controls.hpp"
#include "../matting/background_cut/background_cut_matter.hpp"
#include "../matting/background_negation_matter.hpp"
#include "../matting/matter.hpp"
#include "matter_state.hpp"

#include <pistache/router.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

using namespace Pistache;
using namespace std;

class MatterHandler {
public:
  MatterHandler(
      OpenCVWebcam &webcam, OpenCVWebcamControls &webcam_controls,
      IMatter *&matter, std::string &initial_matter, std::mutex &matter_mutex,
      const cv::Mat &clean_plate);
  void setup_routes(Rest::Router &router);
  void cleanup();

private:
  OpenCVWebcam &webcam;
  OpenCVWebcamControls &webcam_controls;
  IMatter *&matter;
  std::string curr_matter;
  std::unique_lock<std::mutex> matter_lock;
  cv::Mat clean_plate;

  map<const IMatterMode *, MatterState> matter_states;

  void get_matters(const Rest::Request &request, Http::ResponseWriter response);
  void set_matter(const Rest::Request &request, Http::ResponseWriter response);
  void
  update_config(const Rest::Request &request, Http::ResponseWriter response);
  void
  take_clean_plate(const Rest::Request &request, Http::ResponseWriter response);
};

#endif
