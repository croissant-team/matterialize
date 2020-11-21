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
    MatterConfigField field;

    try {
      field = fields_map.at(name);
    } catch (const out_of_range &e) {
      string err_msg{name + " is not a valid config field for given matter"};
      throw invalid_argument(err_msg);
    }

    this->set_field(name, value);
    must_reinit |= field.requires_init;
  }

  return must_reinit;
}

void MatterConfig::set_field(string field_name, string value) {
  assert(config_document.HasMember(field_name.c_str()));
  auto value_json = &config_document[field_name.c_str()];
  value_json->SetString(value.c_str(), config_document.GetAllocator());
}

MatterConfig MatterConfig::default_for(const IMatterMode *mode) {
  return MatterConfig(mode->config_fields());
}