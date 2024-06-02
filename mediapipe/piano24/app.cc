#include <iostream>
#include <thread>

#include "threads/threads.h"

int main(int argc, char** argv) {
  std::cout << "Hello World!";

  SafeQueue<MidiOutQueueElem> q_in_midiout;

  std::thread midi_in(midiin_thread, q_in_midiout)

  return 0;    
}
