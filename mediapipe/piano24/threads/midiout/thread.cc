#include <iostream>

#include "../threads.h";

void midiout_thread( SafeQueue<MidiOutQueueElem>& queue_in ) {
    while (true) {
        MidiOutQueueElem event = queue_in.dequeue();
        std::cout << (int)event.byte0 << " " << (int)event.byte1 << " " << (int)event.byte2 << "\n";
    }
}
