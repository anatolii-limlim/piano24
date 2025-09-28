#include <ctime>
#include <iomanip>
#include <sstream>
#include <filesystem>   // C++17
#include <string>

#include "utils.h"

void saveScreenshot(const cv::Mat& frame) {
    namespace fs = std::filesystem;

    // Ensure screenshots folder exists
    std::string folder = "screenshots";
    if (!fs::exists(folder)) {
        fs::create_directory(folder);
    }

    // Get current time
    auto t = std::time(nullptr);
    std::tm tm;
#ifdef _WIN32
    localtime_s(&tm, &t); // Windows
#else
    localtime_r(&t, &tm); // Linux/Unix
#endif

    // Format filename: frame_YYYY-MM-DD_HH-MM-SS.jpg
    std::ostringstream oss;
    oss << folder << "/frame_"
        << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S")
        << ".jpg";

    std::string filename = oss.str();

    // Save image
    if (!cv::imwrite(filename, frame)) {
        std::cerr << "Failed to save image: " << filename << std::endl;
    } else {
        std::cout << "Saved: " << filename << std::endl;
    }
}
