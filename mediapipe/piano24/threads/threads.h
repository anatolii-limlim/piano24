#include <cstddef>

#include "safe_queue.h"

struct MidiOutQueueElem {
    enum MidiOutInputType { MidiIn, Pitch };
    MidiOutInputType type; 
    
    // if type == MidiIn
    std::byte byte1, byte2, byte3;
    
    // if type == Pitch
    std::byte note;
    double pitch;
};

void midiin_thread();
void midiout_thread();
