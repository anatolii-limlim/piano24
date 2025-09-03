#include "PianoCoordinateTransformer.h"

PianoCoordinateTransformer::PianoCoordinateTransformer(
    const cv::Mat& ethalonImage,
    const CornerCoords& ethalonCorners)
    : dictionary_(cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50)),
      ethalonCorners_(ethalonCorners)
{
    // Detect ARUCO markers in ethalon image
    valid_ = detectMarkers(ethalonImage, ethalonMarkerPositions_);
}

bool PianoCoordinateTransformer::detectMarkers(const cv::Mat& img, std::array<cv::Point2f, 3>& markerCenters)
{
    std::vector<int> ids;
    std::vector<std::vector<cv::Point2f>> corners;
    cv::aruco::detectMarkers(img, dictionary_, corners, ids);

    if (ids.size() < 3) return false;

    // Find required markers
    int found = 0;
    for (int i = 0; i < 3; ++i) {
        auto it = std::find(ids.begin(), ids.end(), markerIDs_[i]);
        if (it != ids.end()) {
            int idx = std::distance(ids.begin(), it);
            // Average corners for center
            cv::Point2f center(0, 0);
            for (const auto& pt : corners[idx]) center += pt;
            center *= (1.0f / corners[idx].size());
            markerCenters[i] = center;
            ++found;
        }
    }
    return found == 3;
}
