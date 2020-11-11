#include "camera_endpoint.hpp"
#include "../util/video_devices.hpp"

CameraEndpoint::CameraEndpoint(
    Pistache::Address addr, OpenCVWebcam &webcam, IMatter *&matter)
    : httpEndpoint(std::make_shared<Pistache::Http::Endpoint>(addr)),
      webcam{webcam},
      matter{matter} {}

void CameraEndpoint::init(size_t thr) {
  auto opts =
      Pistache::Http::Endpoint::options().threads(static_cast<int>(thr));
  httpEndpoint->init(opts);
  setupRoutes();
}

void CameraEndpoint::start() {
  httpEndpoint->setHandler(router.handler());
  httpEndpoint->serve();
}

void CameraEndpoint::setupRoutes() {
  using namespace Pistache::Rest;

  Routes::Post(
      router,
      "/camera/set/:dev_num",
      Routes::bind(&CameraEndpoint::setCamera, this));
  Routes::Get(
      router,
      "/camera/options",
      Routes::bind(&CameraEndpoint::getCameras, this));
  Routes::Post(
      router,
      "/matter/set/:name",
      Routes::bind(&CameraEndpoint::setMatter, this));
  Routes::Get(
      router,
      "/matter/options",
      Routes::bind(&CameraEndpoint::getMatters, this));
  Routes::Post(
      router,
      "/background/set",
      Routes::bind(&CameraEndpoint::setBackground, this));
}

void CameraEndpoint::setCamera(
    const Pistache::Rest::Request &request,
    Pistache::Http::ResponseWriter response) {
  auto dev_num = request.param(":dev_num").as<std::string>();

  try {
    webcam.changeDevice(std::stoi(dev_num));
  } catch (const std::exception &e) {
    response.send(
        Pistache::Http::Code::Bad_Request, "Invalid device number parameter");
    return;
  }

  response.send(Pistache::Http::Code::Ok, "Camera set to /dev/video" + dev_num);
}

void CameraEndpoint::getCameras(
    const Pistache::Rest::Request &request,
    Pistache::Http::ResponseWriter response) {
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

  auto cors_header(
      std::make_shared<Pistache::Http::Header::AccessControlAllowOrigin>("*"));
  response.headers().add(cors_header);
  response.headers().add<Pistache::Http::Header::ContentType>(
      MIME(Application, Json));
  response.send(Pistache::Http::Code::Ok, s.GetString());
}

void CameraEndpoint::setMatter(
    const Pistache::Rest::Request &request,
    Pistache::Http::ResponseWriter response) {
  response.send(Pistache::Http::Code::Ok);
}

void CameraEndpoint::getMatters(
    const Pistache::Rest::Request &request,
    Pistache::Http::ResponseWriter response) {
  const int num_matters = 4;
  const char *all_matters[num_matters] = {
      "KMeans", "Background Negation", "OpenCV", "Background Cut"};

  rapidjson::StringBuffer s;
  rapidjson::Writer<rapidjson::StringBuffer> writer(s);

  writer.StartArray();
  for (int i = 0; i < num_matters; i++) {
    writer.StartObject();
    writer.Key("name");
    writer.String(all_matters[i]);
    writer.Key("id");
    writer.Int(i);
    writer.EndObject();
  }
  writer.EndArray();

  response.send(Pistache::Http::Code::Ok, s.GetString());
}

void CameraEndpoint::setBackground(
    const Pistache::Rest::Request &request,
    Pistache::Http::ResponseWriter response) {
  if (request.hasParam("path"))
    response.send(Pistache::Http::Code::Ok);
}
