#include <cstddef>

#include "safe_queue.h"

enum MidiOutQueueElemType { MidiIn, Pitch };

struct MidiOutQueueElem {
    MidiOutQueueElemType type; 
    
    // if type == MidiIn
    unsigned char byte0, byte1, byte2;
    
    // if type == Pitch
    unsigned char note;
    double pitch;
};

void midiin_thread( SafeQueue<MidiOutQueueElem> &q_in_midiout );
void midiout_thread( SafeQueue<MidiOutQueueElem> &queue_in );
