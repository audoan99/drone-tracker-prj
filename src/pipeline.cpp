#include "pipeline.h"
#include "source.h"
#include "inference.h"
#include "tracker.h"
#include "osd.h"
#include "sink.h"

GstElement* build_pipeline(const PipelineConfig& config) {
    GstElement *pipeline = gst_pipeline_new("drone-tracker-pipeline");

    // 1. Streammux
    GstElement *streammux = gst_element_factory_make("nvstreammux", "stream-muxer");
    g_object_set(G_OBJECT(streammux), "width", config.streammux_width, "height", config.streammux_height, 
                 "batch-size", 1, "batched-push-timeout", 40000, "live-source", 0, NULL);
    gst_bin_add(GST_BIN(pipeline), streammux);

    GstElement *last_element = streammux;

    // 2. Source
    if (config.source_enable) {
        GstElement *source_bin = create_source_bin(0, config.source_uri.c_str(), streammux);
        gst_bin_add(GST_BIN(pipeline), source_bin);
    }

    // 3. PGIE (Conditionally linked)
    if (config.pgie_enable) {
        GstElement *pgie = create_pgie(config.pgie_config_file.c_str());
        gst_bin_add(GST_BIN(pipeline), pgie);
        if (!gst_element_link(last_element, pgie)) return NULL;
        last_element = pgie;
    }

    // 4. Tracker (Conditionally linked - THIS IS THE MAGIC PART!)
    if (config.tracker_enable) {
        GstElement *tracker = create_tracker(config.tracker_ll_config_file.c_str());
        gst_bin_add(GST_BIN(pipeline), tracker);
        if (!gst_element_link(last_element, tracker)) return NULL;
        last_element = tracker;
    }

    // 5. OSD
    if (config.osd_enable) {
        GstElement *osd = create_osd();
        gst_bin_add(GST_BIN(pipeline), osd);
        if (!gst_element_link(last_element, osd)) return NULL;
        last_element = osd;
    }

    // 6. Sink
    if (config.sink_enable) {
        GstElement *sink_bin = create_sink_bin(config.sink_rtsp_port);
        gst_bin_add(GST_BIN(pipeline), sink_bin);
        if (!gst_element_link(last_element, sink_bin)) return NULL;
    }

    return pipeline;
}