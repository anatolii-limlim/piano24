#include <iostream>
#include <cstdlib>
#include <signal.h>
#include <unistd.h>

#include "../../rtmidi/RtMidi.h"
#include "../threads.h"
 
int midiin_thread( SafeQueue<MidiOutQueueElem> q_in_midiout )
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
  std::cout << "Reading MIDI from port ... quit with Ctrl-C.\n";
  while ( true ) {
    stamp = midiin->getMessage( &message );
    nBytes = message.size();
    for ( i=0; i<nBytes; i++ )
      std::cout << "Byte " << i << " = " << (int)message[i] << ", ";
    if ( nBytes > 0 )
      std::cout << "stamp = " << stamp << std::endl;
 
    // Sleep for 1 millisecond ... platform-dependent.
    sleep( 0.001 );
  }
 
  // Clean up
 cleanup:
  delete midiin;
 
  return 0;
}
