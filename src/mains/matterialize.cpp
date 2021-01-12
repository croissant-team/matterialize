#include "../files/app/matterialize_app.hpp"
#include "../files/benchmatte/benchmark.hpp"
#include "../files/util/converter.hpp"

#include <csignal>
#include <v4l2cpp/V4l2Capture.h>

CleanupHandler cleanup_handler{};

void handle_signal(int signal) {
  cleanup_handler.perform_cleanup_actions(signal);
}

void link_signal_handling() {
  std::vector<int> signals{SIGABRT, SIGFPE, SIGILL, SIGINT, SIGSEGV, SIGTERM};
  for (auto &signal : signals) {
    std::signal(signal, handle_signal);
  }
}

int main() {
  cleanup_handler = CleanupHandler();
  link_signal_handling();
  MatterializeApp app{cleanup_handler};
  app.run();
}