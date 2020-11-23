#include "matter_state.hpp"

#include <utility>

void MatterState::config_update(
    map<string, string> config_updates) {
  bool must_reinit = config.update(std::move(config_updates));

  if (must_reinit) {
    reinit_matter();
  }
}
IMatter *MatterState::get_matter() {
  if (matter_instance == nullptr) {
    matter_instance = mode->init_matter(init_data, config);
  }

  return matter_instance;
}

MatterState::~MatterState() {
  delete matter_instance;
}

MatterState::MatterState(
    MatterMode mode, MatterInitData &init_data)
    : mode{mode},
      config{MatterConfig::default_for(mode)},
      init_data{init_data},
      matter_instance{nullptr} {}

void MatterState::reinit_matter() {
  // only performs lazy reinitialization
  delete matter_instance;
  matter_instance = nullptr;
}
const MatterConfig &MatterState::get_config() const {
  return config;
}
