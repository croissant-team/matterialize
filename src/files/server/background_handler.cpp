#include "background_handler.hpp"

BackgroundHandler::BackgroundHandler(
    std::mutex &matter_mutex, const cv::Mat *&bg_mat,
    const cv::Mat &green_screen)
    : matter_lock(matter_mutex, std::defer_lock),
      bg_mat{bg_mat},
      green_screen{green_screen},
      file_bg_mat{green_screen.clone()} {
  this->bg_mat = &green_screen;
}

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
}

void BackgroundHandler::clear_background(
    const Pistache::Rest::Request &request,
    Pistache::Http::ResponseWriter response) {
  auto cors_header(
      std::make_shared<Pistache::Http::Header::AccessControlAllowOrigin>("*"));
  response.headers().add(cors_header);

  matter_lock.lock();
  bg_mat = &green_screen;
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

  cv::resize(new_bg, file_bg_mat, green_screen.size());

  matter_lock.lock();
  bg_mat = &file_bg_mat;
  matter_lock.unlock();

  response.send(Pistache::Http::Code::Ok);
}
