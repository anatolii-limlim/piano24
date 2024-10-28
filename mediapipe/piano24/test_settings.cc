#include <iostream>
#include <thread>
#include <unistd.h>

#include "app/threads.h"

ABSL_FLAG(std::string, settings_path, "", "Path to file with settings");

int main(int argc, char** argv) {
  std::cout << "Hello World!\nQuit with Ctrl-C.\n";

  absl::ParseCommandLine(argc, argv);
  std::string settings_path = absl::GetFlag(FLAGS_settings_path);

  Settings settings;
  settings.load_file(settings_path);

  PianoGeometry piano_geometry;
  piano_geometry.load_settings(settings);

  return 0;    
}
