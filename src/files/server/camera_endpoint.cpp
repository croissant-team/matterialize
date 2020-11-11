#include "camera_endpoint.hpp"
#include "../util/video_devices.hpp"

CameraEndpoint::CameraEndpoint(
    Pistache::Address addr, std::atomic_bool &running, OpenCVWebcam &webcam,
    IMatter *&matter, std::mutex &matter_mutex,
    std::vector<std::pair<std::string, IMatter *>> &matters)
    : httpEndpoint(std::make_shared<Pistache::Http::Endpoint>(addr)),
      running{running},
      webcam{webcam},
      matter{matter},
      matter_lock(matter_mutex, std::defer_lock),
      matters_map{} {
  for (auto &[name, matter] : matters) {
    matters_map[name] = matter;
  }
}

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

void CameraEndpoint::shutdown() {
  httpEndpoint->shutdown();
}

void CameraEndpoint::setupRoutes() {
  using namespace Pistache::Rest;

  Routes::Post(
      router,
      "/camera/set/:dev_num",
      Routes::bind(&CameraEndpoint::set_camera, this));
  Routes::Get(
      router,
      "/camera/options",
      Routes::bind(&CameraEndpoint::get_cameras, this));
  Routes::Post(
      router, "/matter/set", Routes::bind(&CameraEndpoint::set_matter, this));
  Routes::Get(
      router,
      "/matter/options",
      Routes::bind(&CameraEndpoint::get_matters, this));
  Routes::Post(
      router,
      "/background/set",
      Routes::bind(&CameraEndpoint::set_background, this));
  Routes::Post(
      router, "/shutdown", Routes::bind(&CameraEndpoint::do_shutdown, this));
}

void CameraEndpoint::set_camera(
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

void CameraEndpoint::get_cameras(
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

void CameraEndpoint::set_matter(
    const Pistache::Rest::Request &request,
    Pistache::Http::ResponseWriter response) {
  std::string choice{request.body()};

  if (choice.empty()) {
    response.send(Pistache::Http::Code::Bad_Request, "No matter option given");
    return;
  }

  if (matters_map.count(choice) == 0) {
    response.send(Pistache::Http::Code::Bad_Request, "Invalid matter name");
    return;
  }

  matter_lock.lock();
  matter = matters_map[choice];
  matter_lock.unlock();

  response.send(Pistache::Http::Code::Ok, "Matter changed to " + choice);
}

void CameraEndpoint::get_matters(
    const Pistache::Rest::Request &request,
    Pistache::Http::ResponseWriter response) {
  rapidjson::StringBuffer s;
  rapidjson::Writer<rapidjson::StringBuffer> writer(s);

  writer.StartArray();
  for (auto &[name, _] : matters_map) {
    writer.StartObject();
    writer.Key("name");
    writer.String(name.c_str());
    writer.EndObject();
  }
  writer.EndArray();

  auto cors_header(
      std::make_shared<Pistache::Http::Header::AccessControlAllowOrigin>("*"));
  response.headers().add(cors_header);
  response.headers().add<Pistache::Http::Header::ContentType>(
      MIME(Application, Json));
  response.send(Pistache::Http::Code::Ok, s.GetString());
}

void CameraEndpoint::set_background(
    const Pistache::Rest::Request &request,
    Pistache::Http::ResponseWriter response) {
  if (request.hasParam("path"))
    response.send(Pistache::Http::Code::Ok);
}

void CameraEndpoint::do_shutdown(
    const Pistache::Rest::Request &request,
    Pistache::Http::ResponseWriter response) {
  running = false;

  response.send(Pistache::Http::Code::Ok);
}
