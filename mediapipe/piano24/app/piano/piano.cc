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
};
