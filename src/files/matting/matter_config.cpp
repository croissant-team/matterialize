#include "matter_config.hpp"
#include "modes.hpp"

double MatterConfig::get(MatterConfigFieldDouble field) {
  string string_value = json_config[field.name]["value"];
  return stod(string_value);
}

string MatterConfig::get(MatterConfigFieldString field) {
  return json_config[field.name]["value"];
}

// Updates the config an returns true if the updates require the matter
// to be reinitialized
bool MatterConfig::update(const map<string, string> &field_updates) {
  // validate updates
  for (auto &[name, _] : field_updates) {
    if (fields_map.find(name) == fields_map.end()) {
      string err_msg{name + " is not a valid config field for given matter"};
      throw invalid_argument(err_msg);
    }
  }

  // perform updates
  bool must_reinit = false;
  for (auto &[name, value] : field_updates) {
    json_config[name]["value"] = value;
    must_reinit |= fields_map.at(name)->must_reinit_on_update;
  }

  return must_reinit;
}

MatterConfig MatterConfig::default_for(MatterMode mode) {
  return MatterConfig(mode->config_fields());
}
MatterConfig::MatterConfig(vector<const IMatterConfigField *> fields) {
  for (auto &field : fields) {
    fields_map[field->name] = field;
    json_config[field->name]["value"] = field->dump_default_value();
  }
}

void to_json(json &j, const MatterConfig &matter_config) {
  j = matter_config.json_config;
}

void from_json(const json &j, MatterConfig &matter_config) {
  matter_config.json_config = j;
}
json MatterConfig::dump(bool include_field_info) const {
  json config = json_config;
  if (include_field_info) {
    for (const auto &[name, field] : fields_map) {
      config[name]["field_info"] = field->dump_field_info();
    }
  }

  return config;
}

MatterConfigFieldDouble::MatterConfigFieldDouble(
    const char* display_name, const char *name, bool reinit_on_update, double default_value, double min,
    double max, double step_size) noexcept
    : IMatterConfigField{display_name, name, reinit_on_update},
      default_value{default_value},
      min{min},
      max{max},
      step_size{step_size} {}

json MatterConfigFieldString::dump_field_info() const {
  json j = IMatterConfigField::dump_field_info();
  j["default_value"] = default_value;
}
