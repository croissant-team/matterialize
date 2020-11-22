#ifndef MATTERIALIZE_MATTER_HANDLER_HPP
#define MATTERIALIZE_MATTER_HANDLER_HPP

#include "../camera/opencv_webcam.hpp"
#include "../camera/opencv_webcam_controls.hpp"
#include "../matting/background_cut/background_cut_matter.hpp"
#include "../matting/background_negation_matter.hpp"
#include "../matting/manager/matters_manager.hpp"
#include "../matting/matter.hpp"

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
      IMatter *&running_matter_ptr, std::mutex &running_matter_mutex,
      MatterMode initial_matter_mode, const cv::Mat &clean_plate);
  void setup_routes(Rest::Router &router);
  void cleanup();

private:
  OpenCVWebcam &webcam;
  OpenCVWebcamControls &webcam_controls;
  MattersManager matters_manager;

  void get_matters(const Rest::Request &request, Http::ResponseWriter response);
  void set_matter(const Rest::Request &request, Http::ResponseWriter response);
  void
  update_config(const Rest::Request &request, Http::ResponseWriter response);
  void
  take_clean_plate(const Rest::Request &request, Http::ResponseWriter response);
  void get_matters_config(
      const Rest::Request &request, Http::ResponseWriter response);
  void export_config_file(const Rest::Request &request, Http::ResponseWriter response);
  void import_config_file(const Rest::Request &request, Http::ResponseWriter response);
};

#endif
