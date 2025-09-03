#include "PianoCoordinateTransformer.h"

PianoCoordinateTransformer::PianoCoordinateTransformer(const PianoCoordinateTransformer& other)
  : dictionary_(other.dictionary_),
    ethalonCorners_(other.ethalonCorners_),
    ethalonMarkerPositions_(other.ethalonMarkerPositions_),
    keyboardAffine_(other.keyboardAffine_),
    keyboardAffineInv_(other.keyboardAffineInv_),
    markerIDs_(other.markerIDs_),
    valid_(other.valid_)
{
}

void PianoCoordinateTransformer::init(
    const cv::Mat& ethalonImage,
    const CornerCoords& ethalonCorners)
{
    dictionary_ = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);
    ethalonCorners_ = ethalonCorners;

    // Detect ARUCO markers in ethalon image
    valid_ = detectMarkers(ethalonImage, ethalonMarkerPositions_);
}

// Update with new frame (calculate affine from ethalon frame to this frame)
void PianoCoordinateTransformer::updateFrame(const std::array<cv::Point2f, 3>& markerPositions)
{
    keyboardAffine_ = cv::getAffineTransform(ethalonMarkerPositions_, markerPositions);
    keyboardAffineInv_ = cv::getAffineTransform(markerPositions, ethalonMarkerPositions_);

    keyboardAffine_.convertTo(keyboardAffine_, CV_32F);
    keyboardAffineInv_.convertTo(keyboardAffineInv_, CV_32F);
}

void PianoCoordinateTransformer::kbdBasisToPixel(const cv::Point2f& kbdCoords, cv::Point2f& pixel) const {
    if (!valid_) {
        pixel = cv::Point2f(-1, -1);
        return;
    }

    cv::Mat src = (cv::Mat_<float>(1, 3) << kbdCoords.x, kbdCoords.y, 1.0f);
    cv::Mat dst = keyboardAffine_ * src.t();
    pixel.x = dst.at<float>(0, 0);
    pixel.y = dst.at<float>(1, 0);
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
