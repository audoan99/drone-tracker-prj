#include <gst/gst.h>
#include <glib.h>
#include <stdio.h>
#include "pipeline.h"
#include "config_parser.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        g_printerr("Usage: %s <deepstream_app_config.txt>\n", argv[0]);
        return -1;
    }

    const gchar *config_path = argv[1];

    gst_init(&argc, &argv);
    GMainLoop *loop = g_main_loop_new(NULL, FALSE);

    // 1. Parse the config file!
    PipelineConfig config = parse_deepstream_config(config_path);
    
    // 2. Build pipeline based on the parsed config
    GstElement *pipeline = build_pipeline(config);
    if (!pipeline) {
        g_printerr("Failed to build pipeline.\n");
        return -1;
    }

    GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Unable to set pipeline to playing state.\n");
        return -1;
    }

    g_print("\n*** Pipeline Running ***\n");
    g_main_loop_run(loop);

    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    g_main_loop_unref(loop);

    return 0;
}