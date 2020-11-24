#include "matters_manager.hpp"
#include <fstream>

MattersManager::MattersManager(
    IMatter *&running_matter, mutex &running_matter_mutex,
    MatterMode running_matter_mode)
    : running_matter{running_matter},
      running_matter_mode{running_matter_mode},
      running_matter_lock{running_matter_mutex, std::defer_lock} {
  // the MattersManager should be responsible for IMatter initializations
  assert(running_matter == nullptr);

  // initialize matter states
  for (auto mode : MatterModes::modes) {
    matters_state.try_emplace(mode, mode, init_data);
  }

  // initialize running matter
  auto &running_matter_state = matters_state.at(running_matter_mode);
  running_matter_lock.lock();
  running_matter = running_matter_state.get_matter();
  running_matter_lock.unlock();
}

void MattersManager::set_running_mode(MatterMode mode) {
  if (mode == running_matter_mode) {
    return;
  }

  auto &state = matters_state.at(mode);
  auto matter = state.get_matter();

  running_matter_lock.lock();
  running_matter = matter;
  running_matter_mode = mode;
  running_matter_lock.unlock();
}

void MattersManager::update_config(
    MatterMode mode, map<string, string> config_updates) {
  auto &state = matters_state.at(mode);

  if (mode == running_matter_mode) {
    // lock running_matter_lock and make sure it gets unlocked when leaving
    // scope even if exception is thrown by config_update
    std::lock_guard<unique_lock<mutex>> lock_guard{running_matter_lock};

    state.config_update(config_updates);
    // Updating the config might have invalidated the current matter so we get
    // it again
    running_matter = state.get_matter();
  } else {
    state.config_update(config_updates);
  }
}
void MattersManager::update_clean_plate(const cv::Mat &new_clean_plate) {
  // update init data for all matters
  init_data.clean_plate = new_clean_plate;

  // we must (lazily) reinitialize all matters requiring an updated clean plate
  for (auto &[mode, state] : matters_state) {
    if (mode->requires_clean_plate()) {
      if (mode == running_matter_mode) {
        running_matter_lock.lock();
        state.reinit_matter();
        running_matter = state.get_matter();
        running_matter_lock.unlock();
      } else {
        state.reinit_matter();
      }
    }
  }
}
void MattersManager::save_configs(const path &config_file) {
  json configs = dump_matters_config();
  ofstream file{config_file};
  file << configs.dump(4);
  file.close();
}
void MattersManager::load_configs(const path &config_file) {
  ifstream file{config_file};
  json configs = json::parse(file);

  for (auto mode : MatterModes::modes) {
    json config = configs[mode->name()];

    map<string, string> config_fields;
    for (auto &[name, value] : config.items()) {
      config_fields[name] = value;
    }

    update_config(mode, config_fields);
  }

  file.close();
}
void MattersManager::pause_running_matter() {
  assert(!running_matter_lock.owns_lock());
  running_matter_lock.lock();
}
void MattersManager::resume_running_matter() {
  assert(running_matter_lock.owns_lock());
  running_matter_lock.unlock();
}
const MatterConfig &MattersManager::get_matter_config(MatterMode mode) {
  return matters_state.at(mode).get_config();
}
json MattersManager::dump_matters_config() {
  json configs;
  for (const auto &[mode, state] : matters_state) {
    configs[mode->name()] = state.get_config();
  }
  return configs;
}
