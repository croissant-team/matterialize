#include "matter_handler.hpp"
#include "../matting/modes.hpp"
#include "matter_state.hpp"

#define READJUSTMENT_FRAMES 60

using namespace Pistache;

MatterHandler::MatterHandler(
    OpenCVWebcam &webcam, OpenCVWebcamControls &webcam_controls,
    IMatter *&matter, std::string &initial_matter, std::mutex &matter_mutex,
    const cv::Mat &clean_plate)
    : webcam{webcam},
      webcam_controls{webcam_controls},
      matter(matter),
      curr_matter{initial_matter},
      matter_lock(matter_mutex, std::defer_lock),
      clean_plate{clean_plate} {
  for (auto mode : MatterModes::modes) {
    matter_states.try_emplace(mode, mode, clean_plate);
  }
  auto matter_mode = MatterModes::get_by_name(curr_matter);
  auto &matter_state = matter_states.at(matter_mode);
  matter = matter_state.get_matter();
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
  // TODO
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
  for (auto &matter_mode : MatterModes::modes) {
    if (matter_mode->name() != "None") {
      writer.StartObject();
      writer.Key("name");
      writer.String(matter_mode->name().c_str());
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
    const Rest::Request &request, Http::ResponseWriter response) {
  auto cors_header(
      std::make_shared<Http::Header::AccessControlAllowOrigin>("*"));
  response.headers().add(cors_header);

  rapidjson::Document document;
  document.Parse(request.body().c_str());

  if (document.IsNull() || !document.HasMember("matter")) {
    response.send(Http::Code::Bad_Request, "No matter option given");
    return;
  }

  std::string choice{document["matter"].GetString()};

  const IMatterMode *selected_mode;
  try {
    selected_mode = MatterModes::get_by_name(choice);
  } catch (invalid_argument &e) {
    response.send(Http::Code::Bad_Request, e.what());
    return;
  }

  auto &selected_matter_state = matter_states.at(selected_mode);
  auto selected_matter = selected_matter_state.get_matter();

  matter_lock.lock();
  matter = selected_matter;
  matter_lock.unlock();

  response.send(Http::Code::Ok, "Matter changed to " + choice);
}

void MatterHandler::take_clean_plate(
    const Rest::Request &request, Http::ResponseWriter response) {
  auto cors_header(
      std::make_shared<Http::Header::AccessControlAllowOrigin>("*"));
  response.headers().add(cors_header);

  webcam_controls.enable_automatic();
  webcam.roll(READJUSTMENT_FRAMES);
  webcam_controls.disable_automatic();
  clean_plate = webcam.grab();

  //TODO fix problems (at least 2)
  // 1. curr_matter is probably out of date
  // 2. clean_plate_update doesn't get passed a clean plate!!
  for (auto &[matter_mode, matter_state] : matter_states) {
    if (matter_mode->name() == curr_matter) {
      matter_lock.lock();
      matter_state.clean_plate_update();
      matter = matter_state.get_matter();
      matter_lock.unlock();
    } else {
      matter_state.clean_plate_update();
    }
  }

  response.send(
      Pistache::Http::Code::Ok,
      "New clean plate taken and matters initialised");
}

void MatterHandler::update_config(
    const Rest::Request &request, Http::ResponseWriter response) {
  auto cors_header(
      std::make_shared<Http::Header::AccessControlAllowOrigin>("*"));
  response.headers().add(cors_header);

  rapidjson::Document document;
  document.Parse(request.body().c_str());

  if (document.IsNull() || !document.HasMember("matter")) {
    response.send(Http::Code::Bad_Request, "No matter option given");
    return;
  }

  // validate matter mode
  std::string matter_mode{document["matter"].GetString()};
  const IMatterMode *selected_mode;
  try {
    selected_mode = MatterModes::get_by_name(matter_mode);
  } catch (invalid_argument &e) {
    response.send(Http::Code::Bad_Request, e.what());
    return;
  }
  auto &selected_matter_state = matter_states.at(selected_mode);

  if (!document.HasMember("config")) {
    response.send(Http::Code::Bad_Request, "No \"config\" field given");
    return;
  }

  auto config_fields_json = document["config"].GetObject();
  // validate fields
  map<string, string> config_fields{};
  for (auto &field : config_fields_json) {
    if (!field.value.IsString()) {
      response.send(
          Http::Code::Bad_Request,
          "Currently only string config fields are supported");
      return;
    }
    config_fields[field.name.GetString()] = field.value.GetString();
  }

  matter_lock.lock();
  selected_matter_state.config_update(config_fields);
  matter_lock.unlock();
}
