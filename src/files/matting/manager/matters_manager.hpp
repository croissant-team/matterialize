#ifndef MATTERIALIZE_MATTERS_MANAGER_HPP
#define MATTERIALIZE_MATTERS_MANAGER_HPP

#include <exception>
#include <filesystem>
#include <map>

#include "../matter_config.hpp"
#include "../modes.hpp"
#include "matter_state.hpp"

using namespace std;
using namespace filesystem;

// manages the states of all instantiated matters at runtime
class MattersManager {
private:
  IMatter *&running_matter;
  MatterMode running_matter_mode;
  unique_lock<mutex> running_matter_lock;

  // data necessary to initialize matters, shared across matter states
  MatterInitData init_data{};

  map<MatterMode, MatterState> matters_state;

public:
  MattersManager(
      IMatter *&running_matter, mutex &running_matter_mutex,
      MatterMode running_matter_mode);

  struct MissingCleanPlate : public exception {
  public:
    const char *what() const throw() {
      return "Cleanplate not found";
    };
  };

  void pause_running_matter();
  void resume_running_matter();
  void set_running_mode(MatterMode mode);
  void update_config(MatterMode mode, map<string, string> config_updates);
  void update_clean_plate(const cv::Mat &new_clean_plate);
  const MatterConfig &get_matter_config(MatterMode mode);
  json dump_matters_config();
  void save_configs(const path &config_file);
  void load_configs(const path &config_file);
};

#endif// MATTERIALIZE_MATTERS_MANAGER_HPP
