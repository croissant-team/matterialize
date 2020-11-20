#include "matter_state.h"
#include <boost/move/move.hpp>
void MatterState::update_config(map<basic_string<char>, basic_string<char>> config_updates) {
  bool must_reinit = config.update(std::move(config_updates));

  if (must_reinit) {
    delete matter_instance;
    matter_instance = mode.init_matter(init_data, config);
  }
}