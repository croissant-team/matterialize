#include "matter_handler.hpp"
#include "../matting/modes.hpp"

#define READJUSTMENT_FRAMES 60

using namespace Pistache;

MatterHandler::MatterHandler(
    OpenCVWebcam &webcam, OpenCVWebcamControls &webcam_controls,
    IMatter *&running_matter_ptr, std::mutex &running_matter_mutex,
    MatterMode initial_matter_mode, const cv::Mat &clean_plate)
    : webcam{webcam},
      webcam_controls{webcam_controls},
      matters_manager{
          running_matter_ptr,
          running_matter_mutex,
          initial_matter_mode,
          clean_plate} {}

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
  Routes::Post(
      router,
      "/matter/update_config",
      Routes::bind(&MatterHandler::update_config, this));
}

void MatterHandler::cleanup() {
  // Nothing to cleanup (hopefully)
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

  matters_manager.set_running_mode(selected_mode);

  response.send(Http::Code::Ok, "Matter changed to " + choice);
}

void MatterHandler::take_clean_plate(
    const Rest::Request &request, Http::ResponseWriter response) {
  auto cors_header(
      std::make_shared<Http::Header::AccessControlAllowOrigin>("*"));
  response.headers().add(cors_header);

  // take clean plate (pause the running matter to guarantee that this thread
  // is the only one grabbing frames from the webcam)
  matters_manager.pause_running_matter();
  webcam_controls.enable_automatic();
  webcam.roll(READJUSTMENT_FRAMES);
  webcam_controls.disable_automatic();
  cv::Mat clean_plate = webcam.grab();
  matters_manager.resume_running_matter();

  matters_manager.update_clean_plate(clean_plate);

  response.send(
      Http::Code::Ok, "New clean plate taken and matters initialised");
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

  if (!document.HasMember("config")) {
    response.send(Http::Code::Bad_Request, "No \"config\" field given");
    return;
  }

  auto config_fields_json = document["config"].GetObject();
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

  try {
    matters_manager.update_config(selected_mode, config_fields);
  } catch (invalid_argument &e) {
    response.send(Http::Code::Bad_Request, e.what());
    return;
  }

  response.send(
      Http::Code::Ok,
      "Updated specified configuration fields for " + selected_mode->name());
}
void MatterHandler::get_matters_configs(
    const Rest::Request &request, Http::ResponseWriter response) {
  //TODO
}
