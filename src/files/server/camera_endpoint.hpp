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
      Pistache::Address addr, OpenCVWebcam &webcam, IMatter *&matter);

  void init(size_t thr = 2);

  void start();

private:
  void setupRoutes();
  void setCamera(
      const Pistache::Rest::Request &request,
      Pistache::Http::ResponseWriter response);
  void getCameras(
      const Pistache::Rest::Request &request,
      Pistache::Http::ResponseWriter response);
  void setMatter(
      const Pistache::Rest::Request &request,
      Pistache::Http::ResponseWriter response);
  void getMatters(
      const Pistache::Rest::Request &request,
      Pistache::Http::ResponseWriter response);
  void setBackground(
      const Pistache::Rest::Request &request,
      Pistache::Http::ResponseWriter response);

  std::shared_ptr<Pistache::Http::Endpoint> httpEndpoint;
  Pistache::Rest::Router router;
  OpenCVWebcam &webcam;
  IMatter *&matter;
};

#endif// MATTERIALIZE_CAMERA_ENDPOINT_HPP
