#ifndef MATTERIALIZE_MATTER_HANDLER_HPP
#define MATTERIALIZE_MATTER_HANDLER_HPP

#include "../camera/opencv_webcam.hpp"
#include "../matting/background_cut/background_cut_matter.hpp"
#include "../matting/background_negation_matter.hpp"
#include "../matting/matter.hpp"

#include <pistache/router.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

class MatterHandler {
public:
  MatterHandler(
      OpenCVWebcam &webcam, IMatter *&matter, std::mutex &matter_mutex,
      std::vector<std::pair<std::string, IMatter *>> &matters);
  void setup_routes(Pistache::Rest::Router &router);
  void cleanup();

private:
  OpenCVWebcam &webcam;
  IMatter *&matter;
  std::string curr_matter;
  std::unique_lock<std::mutex> matter_lock;
  std::map<std::string, IMatter *> matters_map;

  void get_matters(
      const Pistache::Rest::Request &request,
      Pistache::Http::ResponseWriter response);
  void set_matter(
      const Pistache::Rest::Request &request,
      Pistache::Http::ResponseWriter response);
  void take_clean_plate(
      const Pistache::Rest::Request &request,
      Pistache::Http::ResponseWriter response);
};

#endif
