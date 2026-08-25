#include "sink.h"

// Added rtsp_port parameter
GstElement* create_sink_bin(int rtsp_port) {
    GstElement *sink_bin = gst_bin_new("sink-bin");

    GstElement *nvvidconv = gst_element_factory_make("nvvideoconvert", "sink-nvvidconv");
    GstElement *caps_filter = gst_element_factory_make("capsfilter", "sink-caps-filter");
    GstElement *encoder = gst_element_factory_make("nvv4l2h264enc", "sink-h264-encoder");
    GstElement *parser = gst_element_factory_make("h264parse", "sink-h264-parser");
    GstElement *rtsp_sink = gst_element_factory_make("nvrtspout", "sink-rtsp-out");

    if (!nvvidconv || !caps_filter || !encoder || !parser || !rtsp_sink) {
        g_printerr("Failed to create sink bin elements.\n");
        return NULL;
    }

    GstCaps *caps = gst_caps_from_string("video/x-raw(memory:NVMM), format=NV12");
    g_object_set(G_OBJECT(caps_filter), "caps", caps, NULL);
    gst_caps_unref(caps);

    g_object_set(G_OBJECT(encoder), "bitrate", 4000000, "preset-level", 1, "insert-sps-pps", 1, NULL);
    
    // Use the dynamic RTSP port from the config!
    g_object_set(G_OBJECT(rtsp_sink), "rtsp-port", rtsp_port, "udp-port", 5400, "sync", 0, NULL);

    gst_bin_add_many(GST_BIN(sink_bin), nvvidconv, caps_filter, encoder, parser, rtsp_sink, NULL);
    
    if (!gst_element_link_many(nvvidconv, caps_filter, encoder, parser, rtsp_sink, NULL)) {
        g_printerr("Failed to link sink bin elements.\n");
        return NULL;
    }

    GstPad *pad = gst_element_get_static_pad(nvvidconv, "sink");
    gst_element_add_pad(sink_bin, gst_ghost_pad_new("sink", pad));
    gst_object_unref(pad);

    return sink_bin;
}