#include <fstream>
#include <jsoncpp/json/json.h>

#include "threads.h"

void Settings::load_file( std::string file_name ) {
    Json::Value settings;
    std::ifstream settings_file(file_name, std::fstream::binary);
    settings_file >> settings;

    this->graph_config_path = settings["graph_config_path"].asString();
    this->video_file_path = settings["video_file_path"].asString();
    this->aruco_relative_max_d = settings["aruco_relative_max_d"].asInt();
    this->target_fps = settings["target_fps"].asInt();
    this->admin_app_fps = settings["admin_app_fps"].asInt();
}