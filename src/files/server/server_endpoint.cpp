#include "server_endpoint.hpp"
#include "../util/background_settings.hpp"

ServerEndpoint::ServerEndpoint(
    Pistache::Address addr, std::atomic_bool &running, OpenCVWebcam &webcam,
    OpenCVWebcamControls &webcam_controls, IMatter *&matter, std::string &initial_matter,
    std::mutex &matter_mutex,
    const cv::Mat &clean_plate,
    BackgroundSettings &bg_settings)
    : httpEndpoint(std::make_shared<Pistache::Http::Endpoint>(addr)),
      camera_handler(webcam),
      matter_handler(webcam, webcam_controls, matter, initial_matter, matter_mutex, clean_plate),
      bg_handler(matter_mutex, bg_settings),
      server_handler(running) {}

void ServerEndpoint::init(size_t thr) {
  auto opts = Pistache::Http::Endpoint::options()
                  .flags(Pistache::Tcp::Options::ReuseAddr)
                  .threads(static_cast<int>(thr));
  httpEndpoint->init(opts);
  setup_routes();
}

void ServerEndpoint::start() {
  httpEndpoint->setHandler(router.handler());
  httpEndpoint->serve();
}

void ServerEndpoint::shutdown() {
  matter_handler.cleanup();

  httpEndpoint->shutdown();
}

void ServerEndpoint::setup_routes() {
  using namespace Pistache::Rest;

  camera_handler.setup_routes(router);
  matter_handler.setup_routes(router);
  bg_handler.setup_routes(router);
  server_handler.setup_routes(router);
}
