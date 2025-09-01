#include "PianoCoordinateTransformer.h"

PianoCoordinateTransformer::PianoCoordinateTransformer(
    const cv::Mat& ethalonImage,
    const CornerCoords& ethalonCorners)
    : dictionary_(cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50)),
      ethalonCorners_(ethalonCorners)
{
    // Detect ARUCO markers in ethalon image
    valid_ = detectMarkers(ethalonImage, ethalonMarkerPositions_);

    // Build affine transform for ethalon
    ethalonAffine_ = getAffineTransform(ethalonCorners_);
    ethalonAffineInv_ = getInverseAffineTransform(ethalonAffine_);
}

bool PianoCoordinateTransformer::updateFrame(const cv::Mat& frame)
{
    valid_ = detectMarkers(frame, lastMarkerPositions_);
    if (!valid_) return false;

    // Calculate keyboard corners in current frame using marker positions
    currentCorners_.LT = lastMarkerPositions_[0];
    currentCorners_.RT = lastMarkerPositions_[1];
    currentCorners_.LB = lastMarkerPositions_[2];

    // Homography from ethalon to current
    std::vector<cv::Point2f> src = {
        ethalonCorners_.LT, ethalonCorners_.RT, ethalonCorners_.LB
    };
    std::vector<cv::Point2f> dst = {
        currentCorners_.LT, currentCorners_.RT, currentCorners_.LB
    };
    keyboardHomography_ = cv::findHomography(src, dst, 0);

    // Build affine for current frame
    currentAffine_ = getAffineTransform(currentCorners_);
    currentAffineInv_ = getInverseAffineTransform(currentAffine_);

    return true;
}

bool PianoCoordinateTransformer::pixelToKbdBasis(const cv::Point2f& pixel, cv::Point2f& kbdCoords) const
{
    if (!valid_) return false;
    // Convert pixel to KBD_BASIS in current frame
    cv::Mat src(1, 1, CV_32FC2, (void*)&pixel);
    cv::Mat dst;
    cv::transform(src, dst, currentAffine_);
    kbdCoords = dst.at<cv::Point2f>(0, 0);
    return true;
}

bool PianoCoordinateTransformer::kbdBasisToPixel(const cv::Point2f& kbdCoords, cv::Point2f& pixel) const
{
    if (!valid_) return false;
    // Convert KBD_BASIS to pixel in current frame
    cv::Mat src(1, 1, CV_32FC2, (void*)&kbdCoords);
    cv::Mat dst;
    cv::transform(src, dst, currentAffineInv_);
    pixel = dst.at<cv::Point2f>(0, 0);
    return true;
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

cv::Mat PianoCoordinateTransformer::getAffineTransform(const CornerCoords& corners)
{
    // Map LT -> (0,0), RT -> (1,0), LB -> (0,1)
    std::vector<cv::Point2f> src = { corners.LT, corners.RT, corners.LB };
    std::vector<cv::Point2f> dst = { {0,0}, {1,0}, {0,1} };
    return cv::getAffineTransform(src, dst);
}

cv::Mat PianoCoordinateTransformer::getInverseAffineTransform(const cv::Mat& affine)
{
    // Affine: dst = affine * [x, y, 1]^T
    // To invert: use pseudo-inverse
    cv::Mat inv;
    cv::invertAffineTransform(affine, inv);
    return inv;
}