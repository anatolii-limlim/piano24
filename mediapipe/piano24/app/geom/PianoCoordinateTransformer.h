#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <array>

class PianoCoordinateTransformer {
public:
    struct CornerCoords {
        cv::Point2f LT; // Left Top
        cv::Point2f RT; // Right Top
        cv::Point2f LB; // Left Bottom
    };

    // Constructor: takes ethalon image and keyboard corners
    PianoCoordinateTransformer(const cv::Mat& ethalonImage, const CornerCoords& ethalonCorners);

    // Update with new frame (detect markers and update current transform)
    bool updateFrame(const cv::Mat& frame);

    // Convert from pixel coordinates to KBD_BASIS coordinates (u, v)
    // Returns false if transformation is not available
    bool pixelToKbdBasis(const cv::Point2f& pixel, cv::Point2f& kbdCoords) const;

    // Convert from KBD_BASIS coordinates (u, v) to pixel coordinates
    // Returns false if transformation is not available
    bool kbdBasisToPixel(const cv::Point2f& kbdCoords, cv::Point2f& pixel) const;

    // Returns true if the last updateFrame successfully found all markers
    bool isValid() const { return valid_; }

    // Optionally: get marker positions for debugging
    std::array<cv::Point2f, 3> getLastMarkerPositions() const { return lastMarkerPositions_; }

private:
    // Helper to detect ARUCO markers and get their centers
    bool detectMarkers(const cv::Mat& img, std::array<cv::Point2f, 3>& markerCenters);

    // Calculate transformation from ethalon to current frame
    bool calculateTransform();

    // ARUCO dictionary and marker IDs
    cv::Ptr<cv::aruco::Dictionary> dictionary_;
    std::array<int, 3> markerIDs_ = {8, 9, 47};

    // Ethalon info
    CornerCoords ethalonCorners_;
    std::array<cv::Point2f, 3> ethalonMarkerPositions_;

    // Current frame info
    std::array<cv::Point2f, 3> lastMarkerPositions_;
    CornerCoords currentCorners_;
    bool valid_ = false;

    // Homography from ethalon keyboard corners to current frame keyboard corners
    cv::Mat keyboardHomography_; // 3x3

    // Affine transform from pixel to KBD_BASIS in ethalon
    cv::Mat ethalonAffine_; // 2x3

    // Affine transform from pixel to KBD_BASIS in current frame
    cv::Mat currentAffine_; // 2x3

    // Inverse affine transforms
    cv::Mat ethalonAffineInv_; // 2x3
    cv::Mat currentAffineInv_; // 2x3

    // Helper to build an affine matrix for basis
    static cv::Mat getAffineTransform(const CornerCoords& corners);
    static cv::Mat getInverseAffineTransform(const cv::Mat& affine);
};