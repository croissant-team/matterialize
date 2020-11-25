#include "camera_handler.hpp"

CameraHandler::CameraHandler(OpenCVWebcam &webcam) : webcam{webcam} {}

void CameraHandler::setup_routes(Pistache::Rest::Router &router) {
  using namespace Pistache::Rest;

  Routes::Get(
      router,
      "/camera/options",
      Routes::bind(&CameraHandler::get_cameras, this));
  Routes::Post(
      router, "/camera/set", Routes::bind(&CameraHandler::set_camera, this));
}

void CameraHandler::get_cameras(
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

  std::vector<VideoDevice> devices{VideoDevices::get_devices()};
  for (auto &device : devices) {
    writer.StartObject();
    writer.Key("dev_num");
    writer.Int(device.number);
    writer.Key("name");
    writer.String(device.name.c_str());
    writer.EndObject();
  }

  writer.EndArray();
  writer.EndObject();

  response.headers().add<Pistache::Http::Header::ContentType>(
      MIME(Application, Json));
  response.send(Pistache::Http::Code::Ok, s.GetString());
}
void CameraHandler::set_camera(
    const Pistache::Rest::Request &request,
    Pistache::Http::ResponseWriter response) {
  auto cors_header(
      std::make_shared<Pistache::Http::Header::AccessControlAllowOrigin>("*"));
  response.headers().add(cors_header);

  rapidjson::Document document;
  document.Parse(request.body().c_str());

  if (document.IsNull() || !document.HasMember("dev_num")) {
    response.send(
        Pistache::Http::Code::Bad_Request, "No device number given in body");
    return;
  }

  auto dev_num = document["dev_num"].GetInt();

  bool validNum = false;

  for (const auto &device : VideoDevices::get_devices()) {
    if (device.number == dev_num) {
      validNum = true;
      break;
    }
  }

  if (!validNum) {
    response.send(
        Pistache::Http::Code::Bad_Request, "Invalid device number parameter");
    return;
  }

  try {
    webcam.changeDevice(dev_num);
  } catch (const std::invalid_argument &e) {
    response.send(
        Pistache::Http::Code::Conflict, "Device number not available");
    return;
  }

  response.send(
      Pistache::Http::Code::Ok,
      "Camera set to /dev/video" + std::to_string(dev_num));
}
