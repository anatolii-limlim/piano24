#include <iostream>
#include <cstdlib>
#include <signal.h>
#include <unistd.h>

#include "../../rtmidi/RtMidi.h"
#include "../threads.h"
 
void midi_source_prod( Settings& settings, SafeQueue<InputProcessingQueueElem>& q_input_processing )
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
  
      InputProcessingQueueElem elem {
        type: InputProcessingQueueElem::MidiIn,
        midi_in: MidiInData { byte0: message[0], byte1: message[1], byte2: message[2] }
      };
      q_input_processing.enqueue(elem);
    }

    // Sleep for 1/2 millisecond ... platform-dependent.
    sleep( 0.0005 );
  }
 
  // Clean up
 cleanup:
  delete midiin;
}

void send_midi_data( SafeQueue<InputProcessingQueueElem>& q_input_processing, unsigned char byte0, unsigned char byte1, unsigned char byte2 )
{
  InputProcessingQueueElem elem {
    type: InputProcessingQueueElem::MidiIn,
    midi_in: MidiInData { byte0: byte0, byte1: byte1, byte2: byte2 }
  };
  q_input_processing.enqueue(elem);
}

void midi_source_dev( Settings& settings, SafeQueue<InputProcessingQueueElem>& q_input_processing )
{
  // select program 
  send_midi_data(q_input_processing, 192, 0, 0);
  // change volume
  send_midi_data(q_input_processing, 176, 7, 100);

  while (true) {
    // note on: E3, G5
    send_midi_data(q_input_processing, 144, 52, 60);
    send_midi_data(q_input_processing, 144, 79, 90);
    sleep(3);
    // note off, E3, G5
    send_midi_data(q_input_processing, 128, 52, 90);
    send_midi_data(q_input_processing, 128, 79, 90);

    // note on: C3, E5
    send_midi_data(q_input_processing, 144, 48, 60);
    send_midi_data(q_input_processing, 144, 76, 90);
    sleep(3);
    // note off: C3, E5
    send_midi_data(q_input_processing, 128, 48, 60);
    send_midi_data(q_input_processing, 128, 76, 90);

    // note on: G3, B6
    send_midi_data(q_input_processing, 144, 55, 60);
    send_midi_data(q_input_processing, 144, 83, 90);
    sleep(3);
    // note off: G3, B6
    send_midi_data(q_input_processing, 128, 55, 60);
    send_midi_data(q_input_processing, 128, 83, 90);
    
    // note on: D3, F#4
    send_midi_data(q_input_processing, 144, 50, 60);
    send_midi_data(q_input_processing, 144, 78, 90);
    sleep(3);
    // note off, D3, F#4
    send_midi_data(q_input_processing, 128, 50, 90);
    send_midi_data(q_input_processing, 128, 78, 90);
  }
}

void midi_source_thread( Settings& settings, SafeQueue<InputProcessingQueueElem>& q_input_processing )
{
  if (settings.emulate_midi_source) {
    midi_source_dev(settings, q_input_processing);
  } else {
    midi_source_prod(settings, q_input_processing);
  }
}

