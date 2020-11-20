#ifndef MATTERIALIZE_MATTER_STATE_H
#define MATTERIALIZE_MATTER_STATE_H

#include "../matting/matter_config.hpp"
#include "../matting/matter_mode.hpp"

class MatterState {
  const IMatterMode &mode;
  MatterConfig config;
  MatterInitData &init_data;
  IMatter *matter_instance;

public:
  explicit MatterState(const IMatterMode &mode, MatterInitData &init_data)
      : mode{mode},
        config{MatterConfig::default_for(mode)},
        init_data{init_data} {
    this->matter_instance = mode.init_matter(init_data, config);
  }

  ~MatterState() {
    delete matter_instance;
  }

  // updates each config field according to field_updates, throws
  // invalid_argument if field is not valid for this matter config
  void update_config(std::map<string, string> config_updates);
};
#endif// MATTERIALIZE_MATTER_STATE_H
