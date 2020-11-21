#ifndef MATTERIALIZE_MATTER_MODE_HPP
#define MATTERIALIZE_MATTER_MODE_HPP

#include "matter.hpp"
#include "matter_config.hpp"
#include "matter_init_data.hpp"
#include <string>

using namespace std;

class IMatterMode;
// this has nothing to do with MatterMode which will hopefully be removed
// To add a new matter it should be sufficient to add a matter mode
class IMatterMode {
public:
  [[nodiscard]] virtual const string name() const = 0;
  [[nodiscard]] virtual IMatter *init_matter(MatterInitData &data, MatterConfig &config) const = 0;
  [[nodiscard]] virtual vector<MatterConfigField> config_fields() const = 0;
  [[nodiscard]] virtual bool requires_clean_plate() const = 0;
};

#endif// MATTERIALIZE_MATTER_MODE_HPP
