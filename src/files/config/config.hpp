#ifndef MATTERIALIZE_CONFIG_H
#define MATTERIALIZE_CONFIG_H

#include <filesystem>
#include <stdlib.h>

using namespace std::filesystem;

class Config {
public:
  static path user_path() {
    path home_path = getenv("HOME");
    return home_path / ".matterialize";
  }

  static path default_benchmark_path() {
    return user_path() / "benchmark";
  }

  static path default_matters_config_path() {
    return user_path() / "config.json";
  }
};

#endif// MATTERIALIZE_CONFIG_H
