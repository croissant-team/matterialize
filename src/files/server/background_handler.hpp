#ifndef MATTERIALIZE_BACKGROUND_HANDLER_HPP
#define MATTERIALIZE_BACKGROUND_HANDLER_HPP

#include "../util/background_settings.hpp"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <pistache/router.h>
#include <rapidjson/document.h>

class BackgroundHandler {
public:
  BackgroundHandler(std::mutex &matter_mutex, BackgroundSettings &bg_settings);
  void setup_routes(Pistache::Rest::Router &router);

private:
  std::unique_lock<std::mutex> matter_lock;
  BackgroundSettings &bg_settings;

  void clear_background(
      const Pistache::Rest::Request &request,
      Pistache::Http::ResponseWriter response);
  void set_background(
      const Pistache::Rest::Request &request,
      Pistache::Http::ResponseWriter response);
  void blur_background(
      const Pistache::Rest::Request &request,
      Pistache::Http::ResponseWriter response);
  void desktop_background(
      const Pistache::Rest::Request &request,
      Pistache::Http::ResponseWriter response);
  void get_desktops(
      const Pistache::Rest::Request &request,
      Pistache::Http::ResponseWriter response);

};

#endif