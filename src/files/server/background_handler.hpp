#ifndef MATTERIALIZE_BACKGROUND_HANDLER_HPP
#define MATTERIALIZE_BACKGROUND_HANDLER_HPP

#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <pistache/router.h>
#include <rapidjson/document.h>

class BackgroundHandler {
public:
  BackgroundHandler(
      std::mutex &matter_mutex, const cv::Mat *&bg_mat,
      const cv::Mat &green_screen);
  void setup_routes(Pistache::Rest::Router &router);

private:
  std::unique_lock<std::mutex> matter_lock;
  const cv::Mat *&bg_mat;
  const cv::Mat &green_screen;
  cv::Mat file_bg_mat;

  void clear_background(
      const Pistache::Rest::Request &request,
      Pistache::Http::ResponseWriter response);
  void set_background(
      const Pistache::Rest::Request &request,
      Pistache::Http::ResponseWriter response);
};

#endif