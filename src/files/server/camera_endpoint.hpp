#ifndef MATTERIALIZE_CAMERA_ENDPOINT_HPP
#define MATTERIALIZE_CAMERA_ENDPOINT_HPP

#include "pistache/endpoint.h"
#include "pistache/router.h"
#include "pistache/http.h"

class CameraEndpoint {
public:
  explicit CameraEndpoint(Pistache::Address addr);

  void init(size_t thr = 2);

  void start();

private:
  void setupRoutes();
  void setCamera(const Pistache::Rest::Request &request, Pistache::Http::ResponseWriter response);
  void getCameras(const Pistache::Rest::Request &request, Pistache::Http::ResponseWriter response);
  void setMatter(const Pistache::Rest::Request &request, Pistache::Http::ResponseWriter response);
  void getMatters(const Pistache::Rest::Request &request, Pistache::Http::ResponseWriter response);
  void setBackground(const Pistache::Rest::Request &request, Pistache::Http::ResponseWriter response);
  std::shared_ptr<Pistache::Http::Endpoint> httpEndpoint;
  Pistache::Rest::Router router;
};

#endif//MATTERIALIZE_CAMERA_ENDPOINT_HPP
