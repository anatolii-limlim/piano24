#include <iostream>
#include <cstdlib>
#include <signal.h>
#include <unistd.h>

#include "../../../rtmidi/RtMidi.h"
#include "../../threads.h"
 
void midi_source_thread( SafeQueue<MidiEmitterQueueElem>& q_midi_emitter )
{
  RtMidiIn *midiin = new RtMidiIn();
  std::vector<unsigned char> message;
  int nBytes, i;
  double stamp;
 
  // Check available ports.
  unsigned int nPorts = midiin->getPortCount();
  if ( nPorts == 0 ) {
    std::cout << "No ports available!\n";
    goto cleanup;
  }
  midiin->openPort( 0 );
 
  // Don't ignore sysex, timing, or active sensing messages.
  midiin->ignoreTypes( false, false, false );
 
  // Periodically check input queue.
  std::cout << "Reading MIDI from port...\n";
  while ( true ) {
    stamp = midiin->getMessage( &message );
    nBytes = message.size();
    
    if ( nBytes > 0 ) {
      for ( i=0; i<nBytes; i++ )
        std::cout << "Byte " << i << " = " << (int)message[i] << ", ";
      if ( nBytes > 0 )
        std::cout << "stamp = " << stamp << std::endl;
  
      MidiEmitterQueueElem elem { type: MidiIn, byte0: message[0], byte1: message[1], byte2: message[2] };
      q_midi_emitter.enqueue(elem);
    }

    // Sleep for 1/2 millisecond ... platform-dependent.
    sleep( 0.0005 );
  }
 
  // Clean up
 cleanup:
  delete midiin;
}
