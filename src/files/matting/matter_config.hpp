#ifndef MATTERIALIZE_MATTER_CONFIG_HPP
#define MATTERIALIZE_MATTER_CONFIG_HPP

#include <map>
#include <nlohmann/json.hpp>

#include <string>
#include <vector>

using namespace std;
using namespace nlohmann;

class IMatterMode;

class MatterConfigField {
public:
  const char *name;
  const char *default_value;
  bool requires_init = false;

  bool operator<(const MatterConfigField &other) const {
    return name < other.name;
  }
};

// The config that each specific matter implementation requires from the user.
// The implementer of a matter can retrieve config field values from this object
// at any point while the matter is running so it must be kept in a consistent
// state.
class MatterConfig {
  friend class MatterState;

private:
  map<string, MatterConfigField> fields_map{};
  json json_config;

  // updates config field values
  // returns true iff config update requires matter reinitialization
  // throws invalid_argument if the field names are not valid for this config
  [[nodiscard]] bool update(const map<string, string> &field_updates);
  explicit MatterConfig(vector<MatterConfigField> fields);
  static void to_json(json &j, const MatterConfig &matter_config);
  static void from_json(const json &j, MatterConfig &matterConfig);

public:
  static MatterConfig default_for(const IMatterMode *mode);
  std::string get(MatterConfigField field);
};

#endif// MATTERIALIZE_MATTER_CONFIG_HPP
