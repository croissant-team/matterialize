#include "matters_manager.hpp"

#include "matter_state.hpp"

MattersManager::MattersManager(
    IMatter *&running_matter, mutex &running_matter_mutex,
    MatterMode running_matter_mode, cv::Mat clean_plate)
    : running_matter{running_matter},
      running_matter_mode{running_matter_mode},
      running_matter_lock{running_matter_mutex, std::defer_lock},
      init_data{MatterInitData{std::move(clean_plate)}} {
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
    running_matter_lock.lock();
    state.config_update(config_updates);
    // Updating the config might have invalidated the current matter so we get
    // it again
    running_matter = state.get_matter();
    running_matter_lock.unlock();
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
void MattersManager::export_configs(path config_file) {
  // TODO
}
void MattersManager::load_configs(path config_file) {
  // TODO
}
void MattersManager::pause_running_matter() {
  assert(!running_matter_lock.owns_lock());
  running_matter_lock.lock();
}
void MattersManager::resume_running_matter() {
  assert(running_matter_lock.owns_lock());
  running_matter_lock.unlock();
}
