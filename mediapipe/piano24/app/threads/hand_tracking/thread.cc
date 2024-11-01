#include <time.h>

#include "mediapipe/gpu/gl_calculator_helper.h"
#include "mediapipe/gpu/gpu_buffer.h"
#include "mediapipe/gpu/gpu_shared_data_internal.h"

#include "../../threads.h"

constexpr char kInputStream[] = "input_video";
constexpr char kOutputStream[] = "output_video";

absl::Status hand_tracking_thread(
  Settings& settings,
  FramesData& frames_data,
  SafeQueue<HandTrackingQueueElem>& q_hand_tracking
) {
  ABSL_LOG(INFO) << "Hand tracking start, graph: " << settings.graph_config_path;

  std::string calculator_graph_config_contents;
  MP_RETURN_IF_ERROR(mediapipe::file::GetContents(
      settings.graph_config_path,
      &calculator_graph_config_contents));
  ABSL_LOG(INFO) << "Get calculator graph config contents: "
                 << calculator_graph_config_contents;
  mediapipe::CalculatorGraphConfig config =
      mediapipe::ParseTextProtoOrDie<mediapipe::CalculatorGraphConfig>(
          calculator_graph_config_contents);

  ABSL_LOG(INFO) << "Initialize the calculator graph.";
  mediapipe::CalculatorGraph graph;
  MP_RETURN_IF_ERROR(graph.Initialize(config));

  ABSL_LOG(INFO) << "Initialize the GPU.";
  MP_ASSIGN_OR_RETURN(auto gpu_resources, mediapipe::GpuResources::Create());
  MP_RETURN_IF_ERROR(graph.SetGpuResources(std::move(gpu_resources)));
  mediapipe::GlCalculatorHelper gpu_helper;
  gpu_helper.InitializeForTest(graph.GetGpuResources().get());

  ABSL_LOG(INFO) << "Start running the calculator graph.";
  MP_ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller poller,
                      graph.AddOutputStreamPoller(kOutputStream));
  MP_ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller poller_landmark,
                      graph.AddOutputStreamPoller("hand_landmarks"));
  MP_ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller presence_poller,
                      graph.AddOutputStreamPoller("landmark_presence"));
  MP_ASSIGN_OR_RETURN(mediapipe::OutputStreamPoller handedness_poller,
                      graph.AddOutputStreamPoller("handedness"));
  MP_RETURN_IF_ERROR(graph.StartRun({}));

  std::cout << "GRAPH INITIALIZED\n";

  while (true) {
    HandTrackingQueueElem event = q_hand_tracking.dequeue_all();
    Frame* frame = frames_data.get_frame(event.frame_index);
    if (frame == NULL) {
      continue;
    }
    cv::Mat camera_frame;

    FPS fps;
    
    cv::cvtColor(*frame->mat, camera_frame, cv::COLOR_BGR2RGBA);

    // Wrap Mat into an ImageFrame.
    auto input_frame = absl::make_unique<mediapipe::ImageFrame>(
        mediapipe::ImageFormat::SRGBA, camera_frame.cols, camera_frame.rows,
        mediapipe::ImageFrame::kGlDefaultAlignmentBoundary);
    cv::Mat input_frame_mat = mediapipe::formats::MatView(input_frame.get());
    camera_frame.copyTo(input_frame_mat);

    // Prepare and add graph input packet.
    size_t frame_timestamp_us =
        (double)cv::getTickCount() / (double)cv::getTickFrequency() * 1e6;
    MP_RETURN_IF_ERROR(
        gpu_helper.RunInGlContext([&input_frame, &frame_timestamp_us, &graph,
                                   &gpu_helper]() -> absl::Status {
          // Convert ImageFrame to GpuBuffer.
          auto texture = gpu_helper.CreateSourceTexture(*input_frame.get());
          auto gpu_frame = texture.GetFrame<mediapipe::GpuBuffer>();
          glFlush();
          texture.Release();
          // Send GPU image packet into the graph.
          MP_RETURN_IF_ERROR(graph.AddPacketToInputStream(
              kInputStream, mediapipe::Adopt(gpu_frame.release())
                                .At(mediapipe::Timestamp(frame_timestamp_us))));
          return absl::OkStatus();
        }));

    bool is_left_hand_found = false;
    bool is_right_hand_found = false;
    cv::Point2f *left_hand = new cv::Point2f[21], *right_hand = new cv::Point2f[21];

    std::unique_ptr<mediapipe::ImageFrame> output_frame;

    mediapipe::Packet packet;
    mediapipe::Packet landmark_packet;
    mediapipe::Packet presence_packet;
    mediapipe::Packet handedness_packet;

    if (!poller.Next(&packet)) {
      break;
    }
    
    if (!presence_poller.Next(&presence_packet)) {
       break;
    }    
    auto is_landmark_present = presence_packet.Get<bool>();
    
    if (is_landmark_present) {
      if (!poller_landmark.Next(&landmark_packet)) {
        break;
      }
      auto& multiHandLandmarks = landmark_packet.Get<std::vector<::mediapipe::NormalizedLandmarkList>>();

      if (!handedness_poller.Next(&handedness_packet)) {
        break;
      }
      const auto& handedness = handedness_packet.Get<std::vector<mediapipe::ClassificationList, std::allocator<mediapipe::ClassificationList> >>();
      std::vector<int> handedness_ids;

      for (int i = 0; i < handedness.size(); i ++) {
        auto& h = handedness[i];
        if (h.classification_size() > 0) {
          const mediapipe::Classification & c = h.classification(0);
          handedness_ids.push_back(c.index());
        }
      }

      int i = 0;
      for (const ::mediapipe::NormalizedLandmarkList& normalizedlandmarkList: multiHandLandmarks)
      {
        cv::Point2f *hand;
        if (handedness_ids[i] == HAND_LEFT) {
          is_left_hand_found = true;
          hand = left_hand;
        } else {
          is_right_hand_found = true;
          hand = right_hand;
        }

        for (int j = 0; j < 21; j++) {
          hand[j] = cv::Point2f(normalizedlandmarkList.landmark(j).x(), normalizedlandmarkList.landmark(j).y());
        }

        i++;
      }
    }

    // Convert GpuBuffer to ImageFrame.
    MP_RETURN_IF_ERROR(gpu_helper.RunInGlContext(
        [&packet, &output_frame, &gpu_helper]() -> absl::Status {
          auto& gpu_frame = packet.Get<mediapipe::GpuBuffer>();
          auto texture = gpu_helper.CreateSourceTexture(gpu_frame);
          output_frame = absl::make_unique<mediapipe::ImageFrame>(
              mediapipe::ImageFormatForGpuBufferFormat(gpu_frame.format()),
              gpu_frame.width(), gpu_frame.height(),
              mediapipe::ImageFrame::kGlDefaultAlignmentBoundary);
          gpu_helper.BindFramebuffer(texture);
          const auto info = mediapipe::GlTextureInfoForGpuBufferFormat(
              gpu_frame.format(), 0, gpu_helper.GetGlVersion());
          glReadPixels(0, 0, texture.width(), texture.height(), info.gl_format,
                       info.gl_type, output_frame->MutablePixelData());
          glFlush();
          texture.Release();
          return absl::OkStatus();
        }));

    // Convert back to opencv for display or saving.
    cv::Mat output_frame_mat = mediapipe::formats::MatView(output_frame.get());
    if (output_frame_mat.channels() == 4)
      cv::cvtColor(output_frame_mat, output_frame_mat, cv::COLOR_RGBA2BGR);
    else
      cv::cvtColor(output_frame_mat, output_frame_mat, cv::COLOR_RGB2BGR);

   frames_data.update_hands(
      event.frame_index, true, is_left_hand_found, is_right_hand_found,
      left_hand, right_hand, fps.get_fps()
    );
  }

  return absl::Status();
}
