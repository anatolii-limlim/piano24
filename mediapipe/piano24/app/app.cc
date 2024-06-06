#include <iostream>
#include <thread>

#include "threads.h"

ABSL_FLAG(std::string, settings_path, "", "Path to file with settings");

int app_main(int argc, char** argv) {
  std::cout << "Hello World!\nQuit with Ctrl-C.\n";

  absl::ParseCommandLine(argc, argv);
  std::string settings_path = absl::GetFlag(FLAGS_settings_path);

  Settings settings;
  settings.load_file(settings_path);

  SafeQueue<MidiEmitterQueueElem> q_midi_emitter;
  SafeQueue<HandTrackingQueueElem> q_hand_tracking;

  std::thread t_midi_source(midi_source_thread, std::ref(q_midi_emitter));
  std::thread t_midi_emitter(midi_emitter_thread, std::ref(q_midi_emitter));
  // std::thread t_camera_source(camera_source_thread, std::ref(q_hand_tracking));
  // std::thread t_hand_tracking(hand_tracking_thread, settings.graph_config_path, std::ref(q_hand_tracking));

  t_midi_source.join();
  t_midi_emitter.join();
  // t_camera_source.join();
  // t_hand_tracking.join();

  return 0;    
}
