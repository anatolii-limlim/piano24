#include <iostream>
#include <algorithm>

#include "../../rtmidi/RtMidi.h";

#include "../threads.h";

void midi_emitter_thread(
  Settings& settings,
  SafeQueue<MidiEmitterQueueElem>& q_midi_emitter
) {
  std::vector<unsigned char> message;
  message.push_back(0);
  message.push_back(0);
  message.push_back(0);

  RtMidiOut* midiout[6];

  for (int i = 0; i < 6; i++) {
      midiout[i] = new RtMidiOut();
  }

  // Check available ports.
  int nPorts = midiout[0]->getPortCount();
  if ( nPorts == 0 ) {
    std::cout << "No ports available!\n";
    goto cleanup;
  }

  // Open last 6 available ports.
  std::cout << "Open last 6 available MIDI output ports...\n";
  std::cout << "nPorts: " << nPorts << std::endl;
  for (int i = 0; i < 6; i++) {
    midiout[i]->openPort(std::clamp(nPorts - 6 + i, 0, nPorts - 1));
  }

  while (true) {
    MidiEmitterQueueElem event = q_midi_emitter.dequeue();

    std::cout << "EMITTER got event.type: " << event.type << std::endl;

    if (event.type == MidiEmitterQueueElem::MidiIn) {
      std::cout << "** EMITTER::MidiIn" << " " << (int)event.midi_in.byte0 << " " << (int)event.midi_in.byte1 << " " << (int)event.midi_in.byte2 << std::endl;

      message[0] = event.midi_in.byte0;
      message[1] = event.midi_in.byte1;
      message[2] = event.midi_in.byte2;

      midiout[0]->sendMessage( &message );
    }
  }

cleanup:
  for (int i = 0; i < 6; i++) {
    midiout[i]->closePort();
    delete midiout[i];
  }
}
