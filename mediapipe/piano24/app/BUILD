licenses(["notice"])

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "app",
    srcs = glob(["**/*.cc"]),
    hdrs = glob(["**/*.h"]),
    deps = [
      "//mediapipe/framework:calculator_framework",
      "//mediapipe/framework/formats:image_frame",
      "//mediapipe/framework/formats:image_frame_opencv",
      "//mediapipe/framework/port:file_helpers",
      "//mediapipe/framework/port:opencv_highgui",
      "//mediapipe/framework/port:opencv_imgproc",
      "//mediapipe/framework/port:opencv_video",
      "//mediapipe/framework/port:parse_text_proto",
      "//mediapipe/framework/port:status",
      "//mediapipe/gpu:gl_calculator_helper",
      "//mediapipe/gpu:gpu_buffer",
      "//mediapipe/gpu:gpu_shared_data_internal",
      "//mediapipe/util:resource_util",
      "//mediapipe/graphs/hand_tracking:mobile_calculators",
      "@com_google_absl//absl/flags:flag",
      "@com_google_absl//absl/flags:parse",
      "@com_google_absl//absl/log:absl_log",
      "//mediapipe/piano24/rtmidi:rtmidi",
    ]
)
