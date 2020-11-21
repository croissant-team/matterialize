#ifndef MATTERIALIZE_MATTER_STATE_HPP
#define MATTERIALIZE_MATTER_STATE_HPP

#include "../matting/matter_config.hpp"
#include "../matting/matter_init_data.hpp"
#include "../matting/matter_mode.hpp"

// Keeps track of the state necessary to run a matter of the particular
// IMatterMode
class MatterState {
private:
  const IMatterMode *mode;
  MatterConfig config;
  MatterInitData init_data;
  IMatter *matter_instance;

public:
  explicit MatterState(const IMatterMode *mode, const cv::Mat& clean_plate)
      : mode{mode},
        config{MatterConfig::default_for(mode)},
        init_data{MatterInitData{clean_plate}},
        matter_instance{mode->init_matter(init_data, config)} {};

  ~MatterState() {
    // FIXME clion detects and endless loop here. I think it's lying
    delete matter_instance;
  }

  IMatter *get_matter();

  void clean_plate_update();

  // updates each config field according to field_updates, throws
  // invalid_argument if field is not valid for this matter config
  void config_update(std::map<string, string> config_updates);
};
#endif// MATTERIALIZE_MATTER_STATE_HPP
