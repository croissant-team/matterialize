#ifndef MATTERIALIZE_SERVER_HANDLER_HPP
#define MATTERIALIZE_SERVER_HANDLER_HPP

#include <pistache/router.h>
class ServerHandler {
public:
  explicit ServerHandler(std::atomic_bool &running);
  void setup_routes(Pistache::Rest::Router &router);

private:
  std::atomic_bool &running;

  void shutdown_server(
      const Pistache::Rest::Request &request,
      Pistache::Http::ResponseWriter response);
};

#endif
