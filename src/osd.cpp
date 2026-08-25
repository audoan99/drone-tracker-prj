// src/osd.cpp
#include "osd.h"
#include "probe.h"

GstElement* create_osd()
{
    GstElement *nvosd = gst_element_factory_make("nvdsosd", "nv-onscreendisplay");
    if (!nvosd)
    {
        g_printerr("Failed to create nvdsosd.\n");
        return NULL;
    }

    g_object_set(G_OBJECT(nvosd), "display-clock", 0, NULL);

    // Attach the metadata probe to the sink pad of the OSD
    GstPad *osd_sink_pad = gst_element_get_static_pad(nvosd, "sink");
    if (osd_sink_pad)
    {
        gst_pad_add_probe(osd_sink_pad, GST_PAD_PROBE_TYPE_BUFFER, osd_sink_pad_buffer_probe, NULL, NULL);
        gst_object_unref(osd_sink_pad);
    }

    return nvosd;
}