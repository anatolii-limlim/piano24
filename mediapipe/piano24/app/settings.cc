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
    
    this->ethalon_img_path = settings["ethalon_img_path"].asString();
    this->ethalon_kbd_left_top = cv::Point(
      settings["ethalon_kbd_left_top"][0].asInt(),
      settings["ethalon_kbd_left_top"][1].asInt()
    );
    this->ethalon_kbd_right_top = cv::Point(
      settings["ethalon_kbd_right_top"][0].asInt(),
      settings["ethalon_kbd_right_top"][1].asInt()
    );
    this->ethalon_kbd_right_bottom = cv::Point(
      settings["ethalon_kbd_right_bottom"][0].asInt(),
      settings["ethalon_kbd_right_bottom"][1].asInt()
    );

    this->piano_coord.init(
      cv::imread(this->ethalon_img_path),
      PianoCoordinateTransformer::CornerCoords{
        cv::Point2f(this->ethalon_kbd_left_top),
        cv::Point2f(this->ethalon_kbd_right_top),
        cv::Point2f(this->ethalon_kbd_right_bottom)
      }
    );
}