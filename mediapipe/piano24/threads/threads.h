#include <cstddef>

#include "safe_queue.h"


struct MidiOutInput {
    enum MidiOutInputType { MidiIn };
    MidiOutInputType type; 
    
    std::byte byte1, byte2, byte3;
};
