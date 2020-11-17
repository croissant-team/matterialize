#ifndef MATTERIALIZE_CAMERA_HANDLER_HPP
#define MATTERIALIZE_CAMERA_HANDLER_HPP

#include "../camera/opencv_webcam.hpp"
#include "../util/video_devices.hpp"

#include <pistache/router.h>
#include <rapidjson/pointer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

class CameraHandler {
public:
  CameraHandler(OpenCVWebcam &webcam);
  void setup_routes(Pistache::Rest::Router &router);

private:
  OpenCVWebcam &webcam;

  void get_cameras(
      const Pistache::Rest::Request &request,
      Pistache::Http::ResponseWriter response);
  void set_camera(
      const Pistache::Rest::Request &request,
      Pistache::Http::ResponseWriter response);
};

#endif