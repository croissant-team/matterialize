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

  [[nodiscard]] bool update(map<string, string> field_updates);

  explicit MatterConfig(vector<MatterConfigField> fields);

public:
  static MatterConfig default_for(const IMatterMode *mode);
  std::string get(MatterConfigField field);
};

#endif// MATTERIALIZE_MATTER_CONFIG_HPP
