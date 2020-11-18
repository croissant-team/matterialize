#include "cleanup_handler.hpp"

void CleanupHandler::perform_cleanup_actions(int signal) {
  cout << "Cleanup started\n";
  if (controls) {
    controls->enable_automatic();
  }
  if (server) {
    server->shutdown();
  }
  if (server_thread) {
    server_thread->join();
  }
  if (input) {
    input->stop();
  }
  if (output) {
    output->stop();
  }
  if (preview) {
    preview->stop();
  }
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
