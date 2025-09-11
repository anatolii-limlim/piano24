#include <iostream>

#include "../threads.h";

void midi_emitter_thread(
  Settings& settings,
  SafeQueue<MidiEmitterQueueElem>& q_midi_emitter
) {
    while (true) {
        MidiEmitterQueueElem event = q_midi_emitter.dequeue();

        if (event.type == MidiEmitterQueueElem::MidiIn) {
          std::cout << (int)event.midi_in.byte0 << " " << (int)event.midi_in.byte1 << " " << (int)event.midi_in.byte2 << "\n";
        }
    }
}
