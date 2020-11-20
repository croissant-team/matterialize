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

  void set_field(std::string field_name, std::string value);
  [[nodiscard]] bool update(std::map<std::string, std::string> field_updates);

  explicit MatterConfig(vector<MatterConfigField> fields) {
    for (auto &field : fields) {
      fields_map[field.name] = field;
    }
  }

public:
  static MatterConfig default_for(const IMatterMode &mode);
  std::string get(MatterConfigField field);
};

#endif// MATTERIALIZE_MATTER_CONFIG_HPP
