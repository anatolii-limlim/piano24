package(default_visibility = ["//mediapipe/examples:__subpackages__"])


# Linux only
cc_binary(
    name = "piano24",
    data = [
        "//mediapipe/modules/hand_landmark:hand_landmark_full.tflite",
        "//mediapipe/modules/palm_detection:palm_detection_full.tflite",
    ],
    deps = [
        "//mediapipe/examples/desktop:piano24_run",
        "//mediapipe/graphs/hand_tracking:mobile_calculators",
    ],
)
