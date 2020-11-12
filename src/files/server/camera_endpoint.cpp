#include "camera_endpoint.hpp"

CameraEndpoint::CameraEndpoint(
    Pistache::Address addr, std::atomic_bool &running, OpenCVWebcam &webcam,
    IMatter *&matter, std::mutex &matter_mutex,
    std::vector<std::pair<std::string, IMatter *>> &matters,
    const cv::Mat *&bg_mat, const cv::Mat &green_screen)
    : httpEndpoint(std::make_shared<Pistache::Http::Endpoint>(addr)),
      running{running},
      webcam{webcam},
      matter{matter},
      curr_matter{"None"},
      matter_lock(matter_mutex, std::defer_lock),
      matters_map{},
      bg_mat{bg_mat},
      green_screen{green_screen},
      file_bg_mat{green_screen} {
  for (auto &[name, matter] : matters) {
    matters_map[name] = matter;
  }

  bg_mat = &green_screen;
}

void CameraEndpoint::init(size_t thr) {
  auto opts = Pistache::Http::Endpoint::options()
                  .flags(Pistache::Tcp::Options::ReuseAddr)
                  .threads(static_cast<int>(thr));
  httpEndpoint->init(opts);
  setupRoutes();
}

void CameraEndpoint::start() {
  httpEndpoint->setHandler(router.handler());
  httpEndpoint->serve();
}

void CameraEndpoint::shutdown() {
  for (auto &[_, matter] : matters_map) {
    delete matter;
    matter = nullptr;
  }

  httpEndpoint->shutdown();
}

void CameraEndpoint::setupRoutes() {
  using namespace Pistache::Rest;

  Routes::Post(
      router, "/camera/set", Routes::bind(&CameraEndpoint::set_camera, this));
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
      router,
      "/background/clear",
      Routes::bind(&CameraEndpoint::clear_background, this));
  Routes::Post(
      router,
      "/cleanplate/take",
      Routes::bind(&CameraEndpoint::take_clean_plate, this));
  Routes::Post(
      router, "/shutdown", Routes::bind(&CameraEndpoint::do_shutdown, this));
}

void CameraEndpoint::set_camera(
    const Pistache::Rest::Request &request,
    Pistache::Http::ResponseWriter response) {
  rapidjson::Document document;
  document.Parse(request.body().c_str());

  if (document.IsNull() || !document.HasMember("dev_num")) {
    response.send(
        Pistache::Http::Code::Bad_Request, "No device number given in body");
    return;
  }

  auto dev_num = document["dev_num"].GetInt();

  try {
    webcam.changeDevice(dev_num);
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
  rapidjson::Document document;
  document.Parse(request.body().c_str());

  if (document.IsNull() || !document.HasMember("content")) {
    response.send(Pistache::Http::Code::Bad_Request, "No matter option given");
    return;
  }

  std::string choice{document["content"].GetString()};

  if (matters_map.count(choice) == 0) {
    response.send(Pistache::Http::Code::Bad_Request, "Invalid matter name");
    return;
  }

  matter_lock.lock();
  curr_matter = choice;
  matter = matters_map[choice];
  matter_lock.unlock();

  response.send(Pistache::Http::Code::Ok, "Matter changed to " + choice);
}

void CameraEndpoint::get_matters(
    const Pistache::Rest::Request &request,
    Pistache::Http::ResponseWriter response) {
  rapidjson::StringBuffer s;
  rapidjson::Writer<rapidjson::StringBuffer> writer(s);

  writer.StartObject();
  writer.Key("matters");
  writer.StartArray();
  for (auto &[name, _] : matters_map) {
    if (name != "None") {
      writer.StartObject();
      writer.Key("name");
      writer.String(name.c_str());
      writer.EndObject();
    }
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

void CameraEndpoint::set_background(
    const Pistache::Rest::Request &request,
    Pistache::Http::ResponseWriter response) {
  rapidjson::Document document;
  document.Parse(request.body().c_str());

  if (document.IsNull() || !document.HasMember("content")) {
    response.send(
        Pistache::Http::Code::Bad_Request, "No background path given in body");
    return;
  }

  std::string bg_path = document["content"].GetString();

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

void CameraEndpoint::clear_background(
    const Pistache::Rest::Request &request,
    Pistache::Http::ResponseWriter response) {
  matter_lock.lock();
  bg_mat = &green_screen;
  matter_lock.unlock();

  response.send(Pistache::Http::Code::Ok);
}

void CameraEndpoint::take_clean_plate(
    const Pistache::Rest::Request &request,
    Pistache::Http::ResponseWriter response) {
  const cv::Mat new_plate{webcam.grab()};

  matter_lock.lock();
  auto bg_negate_matter{new BackgroundNegationMatter(new_plate)};
  auto bg_cut_matter{new BackgroundCutMatter(new_plate)};

  auto old_bg_negate{matters_map["Background Negation"]};
  auto old_bg_cut{matters_map["Background Cut"]};

  matters_map["Background Negation"] = bg_negate_matter;
  matters_map["Background Cut"] = bg_cut_matter;

  delete old_bg_negate;
  delete old_bg_cut;

  matter = matters_map[curr_matter];

  matter_lock.unlock();

  response.send(
      Pistache::Http::Code::Ok,
      "New clean plate taken and matters initialised");
}

void CameraEndpoint::do_shutdown(
    const Pistache::Rest::Request &request,
    Pistache::Http::ResponseWriter response) {
  running = false;

  response.send(Pistache::Http::Code::Ok);
}
