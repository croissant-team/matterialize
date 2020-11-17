#ifndef MATTERIALIZE_SERVER_ENDPOINT_HPP
#define MATTERIALIZE_SERVER_ENDPOINT_HPP

#include "../camera/opencv_webcam.hpp"
#include "../matting/background_cut/background_cut_matter.hpp"
#include "../matting/background_negation_matter.hpp"
#include "../matting/matter.hpp"
#include "../util/video_devices.hpp"
#include "background_handler.hpp"
#include "camera_handler.hpp"
#include "matter_handler.hpp"
#include "server_handler.hpp"

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

class ServerEndpoint {
public:
  explicit ServerEndpoint(
      Pistache::Address addr, std::atomic_bool &running, OpenCVWebcam &webcam,
      IMatter *&matter, std::mutex &matter_mutex,
      std::vector<std::pair<std::string, IMatter *>> &matters,
      const cv::Mat *&bg_mat, const cv::Mat &green_screen);

  void init(size_t thr = 2);

  void start();

  void shutdown();

private:
  void setup_routes();

  std::shared_ptr<Pistache::Http::Endpoint> httpEndpoint;
  Pistache::Rest::Router router;

  CameraHandler camera_handler;
  MatterHandler matter_handler;
  BackgroundHandler bg_handler;
  ServerHandler server_handler;
};

#endif
