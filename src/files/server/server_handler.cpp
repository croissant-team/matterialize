#include "server_handler.hpp"

ServerHandler::ServerHandler(std::atomic_bool &running) : running{running} {}

void ServerHandler::setup_routes(Pistache::Rest::Router &router) {
  using namespace Pistache::Rest;

  Routes::Post(
      router, "/shutdown", Routes::bind(&ServerHandler::shutdown_server, this));
}
void ServerHandler::shutdown_server(
    const Pistache::Rest::Request &request,
    Pistache::Http::ResponseWriter response) {
  auto cors_header(
      std::make_shared<Pistache::Http::Header::AccessControlAllowOrigin>("*"));
  response.headers().add(cors_header);

  running = false;

  response.send(Pistache::Http::Code::Ok);
}
