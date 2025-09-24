nohup jackd -d alsa --device hw:1 --rate 44100 --period 256 &

fluidsynth --server -g 5 --audio-driver=jack --connect-jack-outputs /usr/share/sounds/sf2/FluidR3_GM.sf2 & disown


# #!/bin/bash

# # Base MIDI port
# BASE_MIDI_PORT=9801
# # Base TCP control port
# BASE_TCP_PORT=9900
# # Soundfont path
# SF2_PATH="/usr/share/sounds/sf2/FluidR3_GM.sf2"
# # File to store PIDs
# PID_FILE="fluidsynth_pids.txt"

# # Remove old PID file
# rm -f "$PID_FILE"

# # Launch 6 instances
# for i in $(seq 1 6); do
#     MIDI_PORT=$((BASE_MIDI_PORT + i - 1))
#     TCP_PORT=$((BASE_TCP_PORT + i - 1))
#     NAME="fs$i"
#     echo "Starting fluidsynth instance $i on MIDI port $MIDI_PORT, TCP port $TCP_PORT..."
#     # run detached, no terminal, no input
#     fluidsynth --server --audio-driver=jack --connect-jack-outputs /usr/share/sounds/sf2/FluidR3_GM.sf2 \
#         >/dev/null 2>&1 < /dev/null & disown
#     echo $! >> "$PID_FILE"
# done  

# echo "All fluidsynth instances started. PIDs saved to $PID_FILE."
