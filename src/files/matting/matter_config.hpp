#ifndef MATTERIALIZE_MATTER_CONFIG_HPP
#define MATTERIALIZE_MATTER_CONFIG_HPP

#include <map>
#include <nlohmann/json.hpp>

#include <string>
#include <vector>

using namespace std;
using namespace nlohmann;

class IMatterMode;
class MatterConfig;

class IMatterConfigField {
  friend class MatterConfig;

private:
  const char *display_name;
  const char *name;
  const bool must_reinit_on_update = false;

  [[nodiscard]] virtual json dump_default_value() const = 0;

protected:
  [[nodiscard]] virtual json dump_field_info() const {
    json j;
    j["display_name"] = display_name;
    j["name"] = name;
    j["must_reinit_on_update"] = must_reinit_on_update;

    return j;
  }

  IMatterConfigField(const char *display_name, const char *name, bool must_reinit_on_update)
      : display_name{display_name}, name{name}, must_reinit_on_update{must_reinit_on_update} {};
};

struct MatterConfigFieldString : public IMatterConfigField {
private:
  const char *default_value;

  [[nodiscard]] json dump_default_value() const override {
    return default_value;
  }
  [[nodiscard]] json dump_field_info() const override;

public:
  MatterConfigFieldString(
      const char *display_name, const char *name, const char, bool must_reinit, const char *default_value)
      : IMatterConfigField{display_name, name, must_reinit}, default_value{default_value} {}
};

class MatterConfigFieldDouble : public IMatterConfigField {
private:
  const double default_value;
  const double min;
  const double max;
  const double step_size;

  [[nodiscard]] json dump_default_value() const override {
    return to_string(default_value);
  };

  [[nodiscard]] json dump_field_info() const override {
    json j = IMatterConfigField::dump_field_info();
    j["default_value"] = default_value;
    j["min"] = min;
    j["max"] = max;
    j["step_size"] = step_size;

    return j;
  }

public:
  MatterConfigFieldDouble(
      const char *display_name, const char *name, bool reinit_on_update, double default_value, double min,
      double max, double step_size) noexcept;
};

class MatterConfig;

// The config that each specific matter implementation requires from the user.
// The implementer of a matter can retrieve config field values from this object
// at any point while the matter is running so it must be kept in a consistent
// state.
class MatterConfig {
  friend class MatterState;
  friend void to_json(json &j, const MatterConfig &matter_config);
  friend void from_json(const json &j, MatterConfig &matter_config);

private:
  map<string, const IMatterConfigField *> fields_map{};
  json json_config;

  // updates config field values
  // returns true iff config update requires matter reinitialization
  // throws invalid_argument if the field names are not valid for this config
  [[nodiscard]] bool update(const map<string, string> &field_updates);
  explicit MatterConfig(vector<const IMatterConfigField *> fields);

public:
  static MatterConfig default_for(const IMatterMode *mode);
  double get(MatterConfigFieldDouble field);
  std::string get(MatterConfigFieldString field);
  json dump(bool include_field_info = false) const;
};

#endif// MATTERIALIZE_MATTER_CONFIG_HPP
