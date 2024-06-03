#include <iostream>
#include <thread>

#include "threads/threads.h"

int main(int argc, char** argv) {
  std::cout << "Hello World!\nQuit with Ctrl-C.\n";

  SafeQueue<MidiOutQueueElem> q_in_midiout;

  // std::thread t_midi_in(midiin_thread, std::ref(q_in_midiout));
  // std::thread t_midi_out(midiout_thread, std::ref(q_in_midiout));
  std::thread t_cam_input(cam_input_thread);

  // t_midi_in.join();
  // t_midi_out.join();
  t_cam_input.join();

  return 0;    
}
