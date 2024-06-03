#include <iostream>
#include <thread>

#include "threads.h"

ABSL_FLAG(std::string, calculator_graph_config_file, "",
          "Name of file containing text format CalculatorGraphConfig proto.");

int app_main(int argc, char** argv) {
  std::cout << "Hello World!\nQuit with Ctrl-C.\n";

  absl::ParseCommandLine(argc, argv);
  std::string graph_config_file = absl::GetFlag(FLAGS_calculator_graph_config_file);

  SafeQueue<MidiEmitterQueueElem> q_midi_emitter;
  SafeQueue<HandTrackingQueueElem> q_hand_tracking;

  std::thread t_midi_source(midi_source_thread, std::ref(q_midi_emitter));
  std::thread t_midi_emitter(midi_emitter_thread, std::ref(q_midi_emitter));
  std::thread t_camera_source(camera_source_thread, std::ref(q_hand_tracking));
  std::thread t_hand_tracking(hand_tracking_thread, std::ref(graph_config_file), std::ref(q_hand_tracking));

  t_midi_source.join();
  t_midi_emitter.join();
  t_camera_source.join();
  t_hand_tracking.join();

  return 0;    
}
