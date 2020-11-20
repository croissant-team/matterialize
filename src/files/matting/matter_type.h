#ifndef MATTERIALIZE_MATTER_TYPE_H
#define MATTERIALIZE_MATTER_TYPE_H

#include "background_negation_matter.hpp"
#include "matter.hpp"
#include <map>
#include <rapidjson/document.h>
#include <string>

using namespace std;

class MatterConfigField {
public:
  const char *name;
  const char *default_value;
  bool requires_init = false;

  bool operator<(const MatterConfigField &other) const {
    return name < other.name;
  }
};

class MatterState;

// The config that each specifici matter implementation requires from the user
class MatterConfig {
  friend class MatterState;

private:
  rapidjson::Document config_document{};
  map<string, MatterConfigField> fields_map{};

  explicit MatterConfig(vector<MatterConfigField> fields) {
    for (auto &field : fields) {
      fields_map[field.name] = field;
    }
  }

  void set_field(string field_name, string value);
  [[nodiscard]] bool update(map<string, string> field_updates);

public:
  string get(MatterConfigField field);
};

class CleanPlate {
public:
  const cv::Mat &mat;

  explicit CleanPlate(cv::Mat &mat) : mat{mat} {};
};

// The initialization data we give to the matter (this is the same for all
// matters) the matters can decide to ignore it or use it
class MatterInitData {
public:
  CleanPlate cleanPlate;
};

// this has nothing to do with MatterMode which will hopefully be removed
// To add a new matter it should be sufficient to add a matter mode
class IMatterMode {
public:
  [[nodiscard]] virtual string name() const = 0;
  [[nodiscard]] virtual vector<MatterConfigField> config_fields() const = 0;
  virtual IMatter *init_matter(MatterInitData &data, MatterConfig &config) = 0;
};

// Example of how an implementer would create matter mode
class BackgroundNegationMode : IMatterMode {
  [[nodiscard]] string name() const override {
    return "Background Negation";
  }

  [[nodiscard]] vector<MatterConfigField> config_fields() const override {
    return {};
  }

  [[nodiscard]] IMatter *init_matter(MatterInitData &data, MatterConfig &config) override {
    return new BackgroundNegationMatter(data.cleanPlate.mat);
  }
};

class MatterState {
  IMatterMode &mode;
  MatterConfig config;
  MatterInitData &init_data;

  // TODO check with Devesh what the type of this needs to be;
  IMatter *matter_instance;

public:
  explicit MatterState(IMatterMode &mode, MatterInitData &init_data)
      : mode{mode}, config{mode.config_fields()}, init_data{init_data} {
    this->matter_instance = mode.init_matter(init_data, config);
  }

  ~MatterState() {
    delete matter_instance;
  }

  // updates each config field according to field_updates, throws
  // invalid_argument if field is not valid for this matter config
  void update_config(map<string, string> config_updates) {
    bool must_reinit = config.update(std::move(config_updates));

    if (must_reinit) {
      delete matter_instance;
      matter_instance = mode.init_matter(init_data, config);
    }
  }
};

#endif// MATTERIALIZE_MATTER_TYPE_H
