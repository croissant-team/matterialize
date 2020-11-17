#include "server_endpoint.hpp"

ServerEndpoint::ServerEndpoint(
    Pistache::Address addr, std::atomic_bool &running, OpenCVWebcam &webcam,
    IMatter *&matter, std::mutex &matter_mutex,
    std::vector<std::pair<std::string, IMatter *>> &matters,
    const cv::Mat *&bg_mat, const cv::Mat &green_screen)
    : httpEndpoint(std::make_shared<Pistache::Http::Endpoint>(addr)),
      camera_handler(webcam),
      matter_handler(webcam, matter, matter_mutex, matters),
      bg_handler(matter_mutex, bg_mat, green_screen),
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
