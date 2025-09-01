#include <fstream>
#include <jsoncpp/json/json.h>

#include "threads.h"

void Settings::load_file( std::string file_name ) {
    Json::Value settings;
    std::ifstream settings_file(file_name, std::fstream::binary);
    settings_file >> settings;

    this->graph_config_path = settings["graph_config_path"].asString();
    this->video_file_path = settings["video_file_path"].asString();
    this->static_frame_path = settings["static_frame_path"].asString();
    this->aruco_relative_max_d = settings["aruco_relative_max_d"].asInt();
    this->target_fps = settings["target_fps"].asInt();
    this->admin_app_fps = settings["admin_app_fps"].asInt();
    
    this->base_img_path = settings["base_img_path"].asString();
    this->base_corner = cv::Point(
      settings["base_corner"][0].asInt(),
      settings["base_corner"][1].asInt()
    );
    this->base_left = cv::Point(
      settings["base_left"][0].asInt(),
      settings["base_left"][1].asInt()
    );
    this->base_bottom = cv::Point(
      settings["base_bottom"][0].asInt(),
      settings["base_bottom"][1].asInt()
    );
}