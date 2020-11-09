#include "camera_endpoint.hpp"

CameraEndpoint::CameraEndpoint(Pistache::Address addr)
    : httpEndpoint(std::make_shared<Pistache::Http::Endpoint>(addr)) {}

void CameraEndpoint::init(size_t thr) {
  auto opts = Pistache::Http::Endpoint::options().threads(static_cast<int>(thr));
  httpEndpoint->init(opts);
  setupRoutes();
}
void CameraEndpoint::start() {
  httpEndpoint->setHandler(router.handler());
  httpEndpoint->serve();
}

void CameraEndpoint::setupRoutes() {
  using namespace Pistache::Rest;

  Routes::Post(router, "/camera/set/:dev_num", Routes::bind(&CameraEndpoint::setCamera, this));
  Routes::Get(router, "/camera/options", Routes::bind(&CameraEndpoint::getCameras, this));
  Routes::Post(router, "/matter/set/:name", Routes::bind(&CameraEndpoint::setMatter, this));
  Routes::Get(router, "/matter/options", Routes::bind(&CameraEndpoint::getMatters, this));
  Routes::Post(router, "/background/set", Routes::bind(&CameraEndpoint::setBackground, this));
}
void CameraEndpoint::setCamera(
    const Pistache::Rest::Request &request,
    Pistache::Http::ResponseWriter response) {
  response.send(Pistache::Http::Code::Ok);
}
void CameraEndpoint::getCameras(
    const Pistache::Rest::Request &request,
    Pistache::Http::ResponseWriter response) {
  response.send(Pistache::Http::Code::Ok);
}
void CameraEndpoint::setMatter(
    const Pistache::Rest::Request &request,
    Pistache::Http::ResponseWriter response) {
  response.send(Pistache::Http::Code::Ok);
}
void CameraEndpoint::getMatters(
    const Pistache::Rest::Request &request,
    Pistache::Http::ResponseWriter response) {
  response.send(Pistache::Http::Code::Ok);
}
void CameraEndpoint::setBackground(
    const Pistache::Rest::Request &request,
    Pistache::Http::ResponseWriter response) {
  response.send(Pistache::Http::Code::Ok);
}
