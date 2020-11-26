#include "cleanup_handler.hpp"

void CleanupHandler::reenable_automatic_controls() {
  cout << "Re-enabling automatic controls\n";
  if (controls) {
    if (input->isAvailable) {
        controls->enable_automatic();
    }

    controls = nullptr;
  }
  cout << "Re-enabled automatic controls\n";
}

void CleanupHandler::shutdown_server() {
  cout << "Shutting down server\n";
  if (server) {
    server->shutdown();
    server = nullptr;
  }
  if (server_thread) {
    server_thread->join();
    server_thread = nullptr;
  }
  cout << "Shut down server\n";
}

void CleanupHandler::stop_cameras() {
  cout << "Stopping cameras\n";
  if (input) {
    input->stop();
    input = nullptr;
  }
  if (output) {
    output->stop();
    output = nullptr;
  }
  if (preview) {
    preview->stop();
    preview = nullptr;
  }
  cout << "Stopped cameras\n";
}

void CleanupHandler::perform_cleanup_actions(int signal) {
  cout << "Cleanup started for signal " << signal << "\n";
  reenable_automatic_controls();
  shutdown_server();
  stop_cameras();
  cout << "Cleanup finished\n";
  exit(signal);
}

void CleanupHandler::set_camera_controls(OpenCVWebcamControls *new_controls) {
  controls = new_controls;
}

void CleanupHandler::set_server_endpoint(ServerEndpoint *new_server) {
  server = new_server;
}

void CleanupHandler::set_server_thread(std::thread *new_server_thread) {
  server_thread = new_server_thread;
}

void CleanupHandler::set_input_webcam(OpenCVWebcam *new_webcam) {
  input = new_webcam;
}

void CleanupHandler::set_output_webcam(FakeWebcam *new_output) {
  output = new_output;
}

void CleanupHandler::set_preview_webcam(FakeWebcam *new_preview) {
  preview = new_preview;
}