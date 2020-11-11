#ifndef MATTERIALIZE_CAMERA_ENDPOINT_HPP
#define MATTERIALIZE_CAMERA_ENDPOINT_HPP

#include "../camera/opencv_webcam.hpp"
#include "../matting/matter.hpp"

#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

class CameraEndpoint {
public:
  explicit CameraEndpoint(
      Pistache::Address addr, std::atomic_bool &running, OpenCVWebcam &webcam,
      IMatter *&matter, std::mutex &matter_mutex,
      std::vector<std::pair<std::string, IMatter *>> &matters);

  void init(size_t thr = 2);

  void start();

  void shutdown();

private:
  void setupRoutes();
  void set_camera(
      const Pistache::Rest::Request &request,
      Pistache::Http::ResponseWriter response);
  void get_cameras(
      const Pistache::Rest::Request &request,
      Pistache::Http::ResponseWriter response);
  void set_matter(
      const Pistache::Rest::Request &request,
      Pistache::Http::ResponseWriter response);
  void get_matters(
      const Pistache::Rest::Request &request,
      Pistache::Http::ResponseWriter response);
  void set_background(
      const Pistache::Rest::Request &request,
      Pistache::Http::ResponseWriter response);
  void do_shutdown(
      const Pistache::Rest::Request &request,
      Pistache::Http::ResponseWriter response);

  std::shared_ptr<Pistache::Http::Endpoint> httpEndpoint;
  std::atomic_bool &running;
  Pistache::Rest::Router router;
  OpenCVWebcam &webcam;
  IMatter *&matter;
  std::unique_lock<std::mutex> matter_lock;
  std::map<std::string, IMatter *> matters_map;
};

#endif
