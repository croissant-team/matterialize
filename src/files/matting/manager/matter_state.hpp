#ifndef MATTERIALIZE_MATTER_STATE_HPP
#define MATTERIALIZE_MATTER_STATE_HPP

#include "../matter_config.hpp"
#include "../modes.hpp"

// Keeps track of the state necessary to run a matter of a particular
// instance of IMatter. Matter instances reinitialization and updates are
// handled lazily.
class MatterState {
private:
  const MatterMode mode;
  MatterConfig config;
  MatterInitData &init_data;
  IMatter *matter_instance;

public:
  explicit MatterState(MatterMode mode, MatterInitData &init_data);
  ~MatterState();

  IMatter *get_matter();
  const MatterConfig &get_config() const;
  void reinit_matter();
  // updates each config field according to field_updates, throws
  // invalid_argument if field is not valid for this matter config
  void config_update(const std::map<string, string> &config_updates);
};

#endif// MATTERIALIZE_MATTER_STATE_HPP
