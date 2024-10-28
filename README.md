## Piano24!

Piano24 is another one try to solve piano fundamental problem of the impossibility of playing _between the notes_ and get effects like pitch bend or vibrato.  

The most popular idea about this is to make the surface of keys sensible that makes musicians able to modify note pitch after key pressed. This is used in synthesizers, piano-like keyboards and even in real pianos.

Project mission is to make these features convenient for use by classic pianists, for this:
* add new pedals to control sensors
* keep keyboard mechanics with no changes, keys shape and weight are important
* ideally if easily attached to any digital piano with smartphone and pc

### Technologies

Any modern digital piano with a full-weighted keyboard.

[MediaPipe](https://mediapipe.readthedocs.io/en/latest/solutions/hands.html) to track finger movement across the key by video stream.

[Aruco markers](https://docs.opencv.org/4.x/d5/dae/tutorial_aruco_detection.html) to track piano position in the frame.

[Linux MIDI processing system](https://packages.debian.org/sid/jackd2) to modify device MIDI stream and sound synthesizing.

First version works with standard computer vision cameras (at least 1280px at 180fps).

### Similar projects

Perfect realization of such a keyboard with hardware sensors is [TouchKeys](https://andrewmcpherson.org/project/touchkeys) by Andrew McPherson. 

### Source code

Source code folder: [mediapipe/piano24/app](mediapipe/piano24/app)