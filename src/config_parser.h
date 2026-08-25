#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include <string>

// Struct to hold all parsed configurations
struct PipelineConfig {
    bool source_enable;
    std::string source_uri;
    
    int streammux_width;
    int streammux_height;

    bool pgie_enable;
    std::string pgie_config_file;

    bool tracker_enable;
    std::string tracker_ll_config_file;

    bool osd_enable;

    bool sink_enable;
    int sink_rtsp_port;
};

// Function to parse the deepstream_app_config.txt
PipelineConfig parse_deepstream_config(const std::string& config_path);

#endif // CONFIG_PARSER_H