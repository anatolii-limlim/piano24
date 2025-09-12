#include <iostream>

#include "../../rtmidi/RtMidi.h";

#include "../threads.h";

void midi_emitter_thread(
  Settings& settings,
  SafeQueue<MidiEmitterQueueElem>& q_midi_emitter
) {
  RtMidiOut *midiout = new RtMidiOut();
  
  std::vector<unsigned char> message;
  message.push_back(0);
  message.push_back(0);
  message.push_back(0);

  // Check available ports.
  unsigned int nPorts = midiout->getPortCount();
  if ( nPorts == 0 ) {
    std::cout << "No ports available!\n";
    delete midiout;
    return;
  }
 
  // Open last available port.
  midiout->openPort(1);

  while (true) {
    MidiEmitterQueueElem event = q_midi_emitter.dequeue();

    if (event.type == MidiEmitterQueueElem::MidiIn) {
      std::cout << "** EMITTER::MidiIn" << " " << (int)event.midi_in.byte0 << " " << (int)event.midi_in.byte1 << " " << (int)event.midi_in.byte2 << std::endl;

      message[0] = event.midi_in.byte0;
      message[1] = event.midi_in.byte1;
      message[2] = event.midi_in.byte2;

      midiout->sendMessage( &message );
    }
  }

  delete midiout;
}
