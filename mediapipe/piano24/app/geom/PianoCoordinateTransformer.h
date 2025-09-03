#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <array>

#ifndef PIANO_COORDINATE_TRANSFORMER
#define PIANO_COORDINATE_TRANSFORMER

class PianoCoordinateTransformer {
public:
    struct CornerCoords {
        cv::Point2f LT; // Left Top
        cv::Point2f RT; // Right Top
        cv::Point2f LB; // Left Bottom
    };

    // Constructors and assignment
    PianoCoordinateTransformer() = default;
    PianoCoordinateTransformer(const PianoCoordinateTransformer& other);

    // Initialize with ethalon image and its keyboard corner coordinates
    void init(const cv::Mat& ethalonImage, const CornerCoords& ethalonCorners);

    // Update with new frame (calculate affine from ethalon frame to this frame)
    void updateFrame(const std::array<cv::Point2f, 3>& markerPositions);

    // Convert from pixel coordinates to KBD_BASIS coordinates (u, v)
    // Returns false if transformation is not available
    void pixelToKbdBasis(const cv::Point2f& pixel, cv::Point2f& kbdCoords) const;

    // Convert from KBD_BASIS coordinates (u, v) to pixel coordinates
    // Returns false if transformation is not available
    void kbdBasisToPixel(const cv::Point2f& kbdCoords, cv::Point2f& pixel) const;

    // Returns true if the last updateFrame successfully found all markers
    bool isValid() const { return valid_; }

private:
    // Helper to detect ARUCO markers and get their centers
    bool detectMarkers(const cv::Mat& img, std::array<cv::Point2f, 3>& markerCenters);

    // ARUCO dictionary and marker IDs
    cv::Ptr<cv::aruco::Dictionary> dictionary_;
    std::array<int, 3> markerIDs_ = {8, 9, 47};

    // Ethalon info
    CornerCoords ethalonCorners_;
    std::array<cv::Point2f, 3> ethalonMarkerPositions_;
    bool valid_ = false;

    // Affine transformations by aruco markers
    cv::Mat keyboardAffine_; // 3x3
    cv::Mat keyboardAffineInv_; // 3x3
};

#endif // PIANO_COORDINATE_TRANSFORMER