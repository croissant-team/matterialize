#ifndef MATTERIALIZE_MATTER_CONFIG_HPP
#define MATTERIALIZE_MATTER_CONFIG_HPP

#include <map>
#include <rapidjson/document.h>
#include <string>
#include <vector>

using namespace std;

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

// The config that each specific matter implementation requires from the user
class MatterConfig {
  friend class MatterState;

private:
  rapidjson::Document config_document{};
  std::map<std::string, MatterConfigField> fields_map{};

  void set_field(string field_name, string value);
  [[nodiscard]] bool update(map<string, string> field_updates);

  explicit MatterConfig(vector<MatterConfigField> fields) {
    for (auto &field : fields) {
      fields_map[field.name] = field;
      set_field(field.name, field.default_value);
    }
  }

public:
  static MatterConfig default_for(const IMatterMode *mode);
  std::string get(MatterConfigField field);
};

#endif// MATTERIALIZE_MATTER_CONFIG_HPP
