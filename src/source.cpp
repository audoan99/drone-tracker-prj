// src/source.cpp
#include "source.h"

static void cb_newpad(GstElement *decodebin, GstPad *decoder_src_pad, gpointer data)
{
    GstCaps *caps = gst_pad_get_current_caps(decoder_src_pad);
    GstStructure *gststruct = gst_caps_get_structure(caps, 0);
    const gchar *gstname = gst_structure_get_name(gststruct);
    GstElement *streammux = (GstElement *)data;

    if (g_str_has_prefix(gstname, "video/x-raw"))
    {
        GstPad *sinkpad = gst_element_get_request_pad(streammux, "sink_0");
        if (gst_pad_link(decoder_src_pad, sinkpad) != GST_PAD_LINK_OK)
        {
            g_printerr("Failed to link decoder to stream muxer.\n");
        }
        gst_object_unref(sinkpad);
    }
    gst_caps_unref(caps);
}

GstElement* create_source_bin(guint index, const gchar *uri, GstElement *streammux)
{
    gchar bin_name[16] = {};
    g_snprintf(bin_name, sizeof(bin_name), "source-bin-%02d", index);
    
    GstElement *nbin = gst_bin_new(bin_name);
    GstElement *uri_decode_bin = gst_element_factory_make("uridecodebin", "uri-decode-bin");

    g_object_set(G_OBJECT(uri_decode_bin), "uri", uri, NULL);
    // Pass streammux directly to the callback to link dynamically
    g_signal_connect(G_OBJECT(uri_decode_bin), "pad-added", G_CALLBACK(cb_newpad), streammux);
    
    gst_bin_add(GST_BIN(nbin), uri_decode_bin);
    return nbin;
}
