#include "matter_handler.hpp"
#include "../matting/none_matter.hpp"
#include "../matting/opencv_matter.hpp"

MatterHandler::MatterHandler(
    OpenCVWebcam &webcam, IMatter *&matter, std::string &initial_matter,
    std::mutex &matter_mutex, const cv::Mat &clean_plate)
    : webcam{webcam},
      matter(matter),
      curr_matter{initial_matter},
      matter_lock(matter_mutex, std::defer_lock),
      type_map{},
      matters_map{},
      clean_plate{clean_plate} {
  type_map["Background Cut"] = MatterMode::BACKGROUND_CUT;
  type_map["Background Negation"] = MatterMode::BACKGROUND_NEGATION;
  type_map["None"] = MatterMode::NONE;
  type_map["OpenCV"] = MatterMode::OPENCV;

  auto initial_type = type_map[initial_matter];
  matters_map[initial_type] = init_matter(initial_type);
  matter = matters_map[initial_type];
}

void MatterHandler::setup_routes(Pistache::Rest::Router &router) {
  using namespace Pistache::Rest;

  Routes::Get(
      router,
      "/matter/options",
      Routes::bind(&MatterHandler::get_matters, this));
  Routes::Post(
      router, "/matter/set", Routes::bind(&MatterHandler::set_matter, this));
  Routes::Post(
      router,
      "/cleanplate/take",
      Routes::bind(&MatterHandler::take_clean_plate, this));
}

void MatterHandler::cleanup() {
  for (auto &[_, matter] : matters_map) {
    delete matter;
    matter = nullptr;
  }
}

IMatter *MatterHandler::init_matter(MatterMode type) {
  switch (type) {
    case MatterMode::NONE:
      return new NoneMatter();
    case MatterMode::BACKGROUND_NEGATION:
      return new BackgroundNegationMatter(clean_plate);
    case MatterMode::OPENCV:
      return new OpenCVMatter();
    case MatterMode::BACKGROUND_CUT:
      return new BackgroundCutMatter(clean_plate);
  }
}

void MatterHandler::get_matters(
    const Pistache::Rest::Request &request,
    Pistache::Http::ResponseWriter response) {
  auto cors_header(
      std::make_shared<Pistache::Http::Header::AccessControlAllowOrigin>("*"));
  response.headers().add(cors_header);

  rapidjson::StringBuffer s;
  rapidjson::Writer<rapidjson::StringBuffer> writer(s);

  writer.StartObject();
  writer.Key("matters");
  writer.StartArray();
  for (auto &[name, _] : type_map) {
    if (name != "None") {
      writer.StartObject();
      writer.Key("name");
      writer.String(name.c_str());
      writer.EndObject();
    }
  }
  writer.EndArray();
  writer.EndObject();

  response.headers().add<Pistache::Http::Header::ContentType>(
      MIME(Application, Json));
  response.send(Pistache::Http::Code::Ok, s.GetString());
}

void MatterHandler::set_matter(
    const Pistache::Rest::Request &request,
    Pistache::Http::ResponseWriter response) {
  auto cors_header(
      std::make_shared<Pistache::Http::Header::AccessControlAllowOrigin>("*"));
  response.headers().add(cors_header);

  rapidjson::Document document;
  document.Parse(request.body().c_str());

  if (document.IsNull() || !document.HasMember("matter")) {
    response.send(Pistache::Http::Code::Bad_Request, "No matter option given");
    return;
  }

  std::string choice{document["matter"].GetString()};

  if (type_map.count(choice) == 0) {
    response.send(Pistache::Http::Code::Bad_Request, "Invalid matter name");
    return;
  }

  curr_matter = choice;
  auto matter_type = type_map[choice];

  if (matters_map.count(matter_type) == 0) {
    matters_map[matter_type] = init_matter(matter_type);
  }

  matter_lock.lock();
  matter = matters_map[matter_type];
  matter_lock.unlock();

  response.send(Pistache::Http::Code::Ok, "Matter changed to " + choice);
}

void MatterHandler::take_clean_plate(
    const Pistache::Rest::Request &request,
    Pistache::Http::ResponseWriter response) {
  auto cors_header(
      std::make_shared<Pistache::Http::Header::AccessControlAllowOrigin>("*"));
  response.headers().add(cors_header);

  clean_plate = webcam.grab();

  matter_lock.lock();

  for (auto i{matters_map.begin()}; i != matters_map.end();) {
    auto j = i;
    i++;

    if (j->second->requires_clean_plate()) {
      delete j->second;
      matters_map.erase(j);
    }
  }

  auto curr_matter_type = type_map[curr_matter];

  if (matters_map.count(curr_matter_type) == 0) {
    matters_map[curr_matter_type] = init_matter(curr_matter_type);
    matter = matters_map[curr_matter_type];
  }

  matter_lock.unlock();

  response.send(
      Pistache::Http::Code::Ok,
      "New clean plate taken and matters initialised");
}
