#include "../threads.h"

void input_processing_thread(
  Settings& settings,
  SafeQueue<InputProcessingQueueElem>& q_input_processing,
  SafeQueue<MidiEmitterQueueElem>& q_midi_emitter
) {
  while (true) {
    InputProcessingQueueElem event = q_input_processing.dequeue();

    if (event.type == InputProcessingQueueElem::MidiIn) {
      q_midi_emitter.enqueue(MidiEmitterQueueElem { type: MidiEmitterQueueElem::MidiIn, midi_in: event.midi_in });
    }
  }
}