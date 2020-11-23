#ifndef MATTERIALIZE_MATTER_MODE_HPP
#define MATTERIALIZE_MATTER_MODE_HPP

#include "matter.hpp"
#include "matter_config.hpp"
#include "matter_init_data.hpp"
#include <string>
#include <set>

using namespace std;

class IMatterMode {
public:
  [[nodiscard]] virtual const string name() const = 0;
  [[nodiscard]] virtual IMatter *init_matter(MatterInitData &data, MatterConfig &config) const = 0;

  //TODO Ideally these should be pointers to MatterConfigField and there should be three
  // vectors to guarantee type safety (of strings, ints and doubles) while currently
  // everything is a string
  [[nodiscard]] virtual vector<MatterConfigField> config_fields() const = 0;
  [[nodiscard]] virtual bool requires_clean_plate() const = 0;
};

#endif// MATTERIALIZE_MATTER_MODE_HPP
