#include "matter_config.hpp"
#include "matter_mode.hpp"

string MatterConfig::get(MatterConfigField field) {
  return config_document[field.name].GetString();
}

// Updates the config an returns true if the updates require the matter
// to be reinitialized
bool MatterConfig::update(map<string, string> field_updates) {
  bool must_reinit = false;

  for (auto &[name, value] : field_updates) {
    if (!config_document.HasMember(name.c_str())) {
      string err_msg{name + " is not a valid config field for given matter"};
      throw invalid_argument(err_msg);
    }

    // Sets field value in json document
    auto &json_field = config_document[name.c_str()];
    json_field.SetString(value.c_str(), config_document.GetAllocator());

    must_reinit |= fields_map[name].requires_init;
  }

  return must_reinit;
}

MatterConfig MatterConfig::default_for(const IMatterMode *mode) {
  return MatterConfig(mode->config_fields());
}
MatterConfig::MatterConfig(vector<MatterConfigField> fields) {
  // initialize json document with default fields values;

  config_document.SetObject();
  for (auto &field : fields) {
    fields_map[field.name] = field;

    // add field to json representation
    rapidjson::Value field_name;
    rapidjson::Value field_default_value;

    field_name.SetString(field.name, config_document.GetAllocator());
    field_default_value.SetString(
        field.default_value, config_document.GetAllocator());

    config_document.AddMember(
        field_name, field_default_value, config_document.GetAllocator());
  }
}
