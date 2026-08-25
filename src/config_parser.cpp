#include "config_parser.h"
#include <gst/gst.h>
#include <stdexcept>

PipelineConfig parse_deepstream_config(const std::string& config_path) {
    PipelineConfig config;
    GKeyFile *key_file = g_key_file_new();
    GError *error = NULL;

    if (!g_key_file_load_from_file(key_file, config_path.c_str(), G_KEY_FILE_NONE, &error)) {
        g_printerr("Failed to load config file %s: %s\n", config_path.c_str(), error->message);
        g_error_free(error);
        g_key_file_free(key_file);
        throw std::runtime_error("Failed to load config");
    }

    // Helper lambdas to safely read values
    auto get_bool = [&](const gchar *group, const gchar *key, bool default_val) -> bool {
        GError *err = NULL;
        gboolean val = g_key_file_get_boolean(key_file, group, key, &err);
        if (err) { g_error_free(err); return default_val; }
        return val;
    };

    auto get_int = [&](const gchar *group, const gchar *key, int default_val) -> int {
        GError *err = NULL;
        int val = g_key_file_get_integer(key_file, group, key, &err);
        if (err) { g_error_free(err); return default_val; }
        return val;
    };

    auto get_string = [&](const gchar *group, const gchar *key, const std::string& default_val) -> std::string {
        GError *err = NULL;
        gchar *val = g_key_file_get_string(key_file, group, key, &err);
        if (err) { g_error_free(err); return default_val; }
        std::string res(val);
        g_free(val);
        return res;
    };

    // Parse the config file based on your deepstream_app_config.txt structure
    config.source_enable = get_bool("source0", "enable", true);
    config.source_uri = get_string("source0", "uri", "file:///opt/nvidia/deepstream/deepstream/samples/streams/sample_720p.mp4");

    config.streammux_width = get_int("streammux", "width", 1280);
    config.streammux_height = get_int("streammux", "height", 720);

    config.pgie_enable = get_bool("primary-gie", "enable", true);
    config.pgie_config_file = get_string("primary-gie", "config-file", "config/config_infer_primary.txt");

    config.tracker_enable = get_bool("tracker", "enable", true);
    config.tracker_ll_config_file = get_string("tracker", "ll-config-file", "config/tracker_config.yml");

    config.osd_enable = get_bool("osd", "enable", true);

    config.sink_enable = get_bool("sink0", "enable", true);
    config.sink_rtsp_port = get_int("sink0", "rtsp-port", 8554);

    g_key_file_free(key_file);
    
    g_print("[Parsed config] Source=%d, PGIE=%d, Tracker=%d, OSD=%d, Sink=%d\n", 
            config.source_enable, config.pgie_enable, config.tracker_enable, config.osd_enable, config.sink_enable);

    return config;
}