#include <iostream>
#include <thread>

#include "threads/threads.h"

ABSL_FLAG(std::string, calculator_graph_config_file, "",
          "Name of file containing text format CalculatorGraphConfig proto.");

int main(int argc, char** argv) {
  std::cout << "Hello World!\nQuit with Ctrl-C.\n";

  absl::ParseCommandLine(argc, argv);
  std::string graph_config_file = absl::GetFlag(FLAGS_calculator_graph_config_file);

  SafeQueue<MidiEmitterQueueElem> q_in_midiout;
  SafeQueue<HandTrackingQueueElem> q_in_hand_tracking;

  // std::thread t_midi_in(midiin_thread, std::ref(q_in_midiout));
  // std::thread t_midi_out(midiout_thread, std::ref(q_in_midiout));
  std::thread t_cam_input(cam_input_thread, std::ref(q_in_hand_tracking));
  std::thread t_hand_tracking(hand_tracking_thread, std::ref(graph_config_file), std::ref(q_in_hand_tracking));

  // t_midi_in.join();
  // t_midi_out.join();
  t_cam_input.join();
  t_hand_tracking.join();

  return 0;    
}
