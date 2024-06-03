#include <iostream>

#include "../../threads.h";

void midi_emitter_thread( SafeQueue<MidiEmitterQueueElem>& q_midi_emitter ) {
    while (true) {
        MidiEmitterQueueElem event = q_midi_emitter.dequeue();
        std::cout << (int)event.byte0 << " " << (int)event.byte1 << " " << (int)event.byte2 << "\n";
    }
}
