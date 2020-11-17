#include "matter_handler.hpp"

MatterHandler::MatterHandler(
    OpenCVWebcam &webcam, IMatter *&matter, std::mutex &matter_mutex,
    std::vector<std::pair<std::string, IMatter *>> &matters)
    : webcam{webcam},
      matter(matter),
      curr_matter{"None"},
      matter_lock(matter_mutex, std::defer_lock),
      matters_map{} {
  for (auto &[name, matter] : matters) {
    matters_map[name] = matter;
  }
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
void MatterHandler::take_clean_plate(
    const Pistache::Rest::Request &request,
    Pistache::Http::ResponseWriter response) {
  auto cors_header(
      std::make_shared<Pistache::Http::Header::AccessControlAllowOrigin>("*"));
  response.headers().add(cors_header);

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
