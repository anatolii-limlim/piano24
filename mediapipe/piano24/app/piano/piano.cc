#include <opencv2/aruco.hpp>
#include <opencv2/aruco/dictionary.hpp>

#include "../threads.h"

PianoGeometry::NotePosition PianoGeometry::get_midi_note_position( int midi_note_id ) {
  midi_note_id -= 12;

  return PianoGeometry::NotePosition {
    octave_id: midi_note_id / 12,
    note_id: midi_note_id % 12
  };
}

void PianoGeometry::load_settings( Settings& settings ) {
  cv::Mat image = cv::imread(settings.base_img_path, 0);

  cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);
  cv::Ptr<cv::aruco::DetectorParameters> parameters = cv::aruco::DetectorParameters::create();
  std::vector<int> markerIds;
  std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;

  cv::aruco::detectMarkers(image, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);

  int start_count = std::count_if(
    markerIds.begin(), markerIds.end(),
    [&](int marker_id) { return marker_id == ARUCO_START; }
  );
  int end_count = std::count_if(
    markerIds.begin(), markerIds.end(),
    [&](int marker_id) { return marker_id == ARUCO_END; }
  );
  
  bool is_pose_detected = start_count == 1 && end_count == 1;

  if (!is_pose_detected) {
    throw "Aruco markers not detected on base_img from settings";
  }

  Basis basis = get_basis_by_aruco(markerIds, markerCorners); 
};

PianoGeometry::Basis PianoGeometry::get_basis_by_aruco(
  std::vector<int>& markerIds,
  std::vector<std::vector<cv::Point2f>>& markerCorners
) {
  auto start_it = std::find(markerIds.begin(), markerIds.end(), ARUCO_START);
  auto end_it = std::find(markerIds.begin(), markerIds.end(), ARUCO_END);
  int start_ind = start_it - markerIds.begin();
  int end_ind = end_it - markerIds.begin();

  using Line2 = Eigen::Hyperplane<float,2>;
  using Vec2  = Eigen::Vector2f;

  std::cout << "HELLO HELLO HELLO HELLO HELLO WORLD " << markerCorners[start_ind][0].x << " " << end_ind << "\n";
}

