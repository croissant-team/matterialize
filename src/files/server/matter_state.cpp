#include "matter_state.hpp"
void MatterState::config_update(map<string, string> config_updates) {
  bool must_reinit = config.update(std::move(config_updates));

  if (must_reinit) {
    delete matter_instance;
    matter_instance = nullptr;
  }
}
IMatter *MatterState::get_matter() {
  if (matter_instance == nullptr) {
    matter_instance = mode->init_matter(MatterInitData{clean_plate}, config);
  }

  return matter_instance;
}
void MatterState::clean_plate_update(const cv::Mat &new_clean_plate) {
  this->clean_plate = new_clean_plate;

  if (matter_instance == nullptr) {
    return;
  }

  if (mode->requires_clean_plate()) {
    delete matter_instance;
    matter_instance = nullptr;
  }
}
