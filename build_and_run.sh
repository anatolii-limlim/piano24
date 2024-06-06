./bazelisk-linux-amd64 build --copt -DMESA_EGL_NO_X11_HEADERS --copt -DEGL_NO_X11 --copt -Wall --copt -D__LINUX_ALSA__  mediapipe/piano24:piano24
bazel-bin/mediapipe/piano24/piano24 --settings_path=mediapipe/piano24/settings_local.json
#bazel-bin/mediapipe/piano24/piano24 --calculator_graph_config_file=mediapipe/graphs/hand_tracking/hand_tracking_desktop_live_gpu.pbtxt
