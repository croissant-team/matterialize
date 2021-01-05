#include "background_handler.hpp"
#include "../util/desktop_capture.hpp"
#include <opencv2/videoio.hpp>
#include <rapidjson/writer.h>

BackgroundHandler::BackgroundHandler(
    std::mutex &matter_mutex, BackgroundSettings &bg_settings)
    : matter_lock(matter_mutex, std::defer_lock), bg_settings{bg_settings} {}

void BackgroundHandler::setup_routes(Pistache::Rest::Router &router) {
  using namespace Pistache::Rest;

  Routes::Post(
      router,
      "/background/clear",
      Routes::bind(&BackgroundHandler::clear_background, this));
  Routes::Post(
      router,
      "/background/set",
      Routes::bind(&BackgroundHandler::set_background, this));
  Routes::Post(
      router,
      "/background/blur",
      Routes::bind(&BackgroundHandler::blur_background, this));
  Routes::Post(
      router,
      "/background/desktop",
      Routes::bind(&BackgroundHandler::desktop_background, this));
  Routes::Get(
      router,
      "/background/desktop/options",
      Routes::bind(&BackgroundHandler::get_desktops, this));
  Routes::Post(
      router,
      "/background/video",
      Routes::bind(&BackgroundHandler::video_background, this));
}

void BackgroundHandler::blur_background(
    const Pistache::Rest::Request &request,
    Pistache::Http::ResponseWriter response) {
  auto cors_header(
      std::make_shared<Pistache::Http::Header::AccessControlAllowOrigin>("*"));
  response.headers().add(cors_header);

  rapidjson::Document document;
  document.Parse(request.body().c_str());

  if (document.IsNull() || !document.HasMember("size")) {
    response.send(
        Pistache::Http::Code::Bad_Request, "No blur size given in body");
    return;
  }

  if (!document["size"].IsInt()) {
    response.send(
        Pistache::Http::Code::Bad_Request, "Blur size must be an integer");
    return;
  }

  int size = document["size"].GetInt();

  if (size <= 0 || size % 2 == 0) {
    response.send(
        Pistache::Http::Code::Bad_Request,
        "Blur size must be an odd positive integer");
    return;
  }

  matter_lock.lock();
  bg_settings.set_blur_size(size);
  bg_settings.mode = BackgroundMode::BLUR;
  matter_lock.unlock();

  response.send(Pistache::Http::Code::Ok);
}

void BackgroundHandler::clear_background(
    const Pistache::Rest::Request &request,
    Pistache::Http::ResponseWriter response) {
  auto cors_header(
      std::make_shared<Pistache::Http::Header::AccessControlAllowOrigin>("*"));
  response.headers().add(cors_header);

  matter_lock.lock();
  bg_settings.mode = BackgroundMode::CLEAR;
  matter_lock.unlock();

  response.send(Pistache::Http::Code::Ok);
}

void BackgroundHandler::set_background(
    const Pistache::Rest::Request &request,
    Pistache::Http::ResponseWriter response) {
  auto cors_header(
      std::make_shared<Pistache::Http::Header::AccessControlAllowOrigin>("*"));
  response.headers().add(cors_header);

  rapidjson::Document document;
  document.Parse(request.body().c_str());

  if (document.IsNull() || !document.HasMember("file_path")) {
    response.send(
        Pistache::Http::Code::Bad_Request, "No background path given in body");
    return;
  }

  std::string bg_path = document["file_path"].GetString();

  cv::Mat new_bg;

  try {
    new_bg = cv::imread(bg_path);
  } catch (const std::exception &e) {
    response.send(Pistache::Http::Code::Bad_Request, e.what());
    return;
  }

  matter_lock.lock();
  auto bg = cv::Mat{};
  cv::resize(new_bg, bg, bg_settings.get_size());
  bg_settings.set_background(bg);
  bg_settings.mode = BackgroundMode::STATIC;
  matter_lock.unlock();

  response.send(Pistache::Http::Code::Ok);
}

void BackgroundHandler::desktop_background(
    const Pistache::Rest::Request &request,
    Pistache::Http::ResponseWriter response) {
  auto cors_header(
      std::make_shared<Pistache::Http::Header::AccessControlAllowOrigin>("*"));
  response.headers().add(cors_header);

  rapidjson::Document document;
  document.Parse(request.body().c_str());

  if (document.IsNull() || !document.HasMember("desktop")) {
    response.send(
        Pistache::Http::Code::Bad_Request, "No desktop given in body");
    return;
  }

  bool found = false;
  std::string target = document["desktop"].GetString();

  matter_lock.lock();
  std::vector<DesktopCapture> desktops{DesktopCaptures::get_desktops()};
  for (const auto &desktop : desktops) {
    if (desktop.name == target) {
      found = true;
      bg_settings.set_desktop(
          desktop.x, desktop.y, desktop.width, desktop.height);
      break;
    }
  }
  if (found) {
    bg_settings.mode = BackgroundMode::DESKTOP;
  }
  matter_lock.unlock();

  if (found) {
    response.send(Pistache::Http::Code::Ok);
  } else {
    response.send(
        Pistache::Http::Code::Bad_Request, "Invalid desktop given in body");
  }
}

void BackgroundHandler::get_desktops(
    const Pistache::Rest::Request &request,
    Pistache::Http::ResponseWriter response) {
  auto cors_header(
      std::make_shared<Pistache::Http::Header::AccessControlAllowOrigin>("*"));
  response.headers().add(cors_header);

  rapidjson::StringBuffer s;
  rapidjson::Writer<rapidjson::StringBuffer> writer(s);

  writer.StartObject();
  writer.Key("devices");
  writer.StartArray();

  std::vector<DesktopCapture> desktops{DesktopCaptures::get_desktops()};
  for (const auto &desktop : desktops) {
    writer.StartObject();
    writer.Key("name");
    writer.String(desktop.name.c_str());
    writer.Key("x");
    writer.Int(desktop.x);
    writer.Key("y");
    writer.Int(desktop.y);
    writer.Key("width");
    writer.Int(desktop.width);
    writer.Key("height");
    writer.Int(desktop.height);
    writer.EndObject();
  }

  writer.EndArray();
  writer.EndObject();

  response.headers().add<Pistache::Http::Header::ContentType>(
      MIME(Application, Json));
  response.send(Pistache::Http::Code::Ok, s.GetString());
}

void BackgroundHandler::video_background(
    const Pistache::Rest::Request &request,
    Pistache::Http::ResponseWriter response) {
  auto cors_header(
      std::make_shared<Pistache::Http::Header::AccessControlAllowOrigin>("*"));
  response.headers().add(cors_header);

  rapidjson::Document document;
  document.Parse(request.body().c_str());

  if (document.IsNull() || !document.HasMember("file_path")) {
    response.send(
        Pistache::Http::Code::Bad_Request, "No video path given in body");
    return;
  }

  std::string video_path = document["file_path"].GetString();

  cv::VideoCapture new_cap{};

  try {
    new_cap = cv::VideoCapture{video_path};
  } catch (const std::exception &e) {
    response.send(Pistache::Http::Code::Bad_Request, e.what());
    return;
  }

  matter_lock.lock();
  bg_settings.set_video(new_cap);
  bg_settings.mode = BackgroundMode::VIDEO;
  matter_lock.unlock();

  response.send(Pistache::Http::Code::Ok);
}
