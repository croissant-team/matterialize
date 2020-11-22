#include "matter_config.hpp"
#include "modes.hpp"

string MatterConfig::get(MatterConfigField field) {
  return json_config[field.name];
  // return config_document[field.name].GetString();
}

// Updates the config an returns true if the updates require the matter
// to be reinitialized
bool MatterConfig::update(const map<string, string> &field_updates) {
  bool must_reinit = false;

  // validate updates
  for (auto &[name, _] : field_updates) {
    if (!json_config.contains(name)) {
      string err_msg{name + " is not a valid config field for given matter"};
      throw invalid_argument(err_msg);
    }
  }

  // perform updates
  for (auto &[name, value] : field_updates) {
    json_config[name] = value;
    must_reinit |= fields_map.at(name).requires_init;
  }

  return must_reinit;
}

MatterConfig MatterConfig::default_for(MatterMode mode) {
  return MatterConfig(mode->config_fields());
}
MatterConfig::MatterConfig(vector<MatterConfigField> fields) {
  for (auto &field : fields) {
    fields_map[field.name] = field;
    json_config[field.name] = field.default_value;
  }
}
void MatterConfig::to_json(json &j, const MatterConfig &matter_config) {
  j = matter_config.json_config;
}
void MatterConfig::from_json(const json &j, MatterConfig &matterConfig) {
  matterConfig.json_config = j;
}
