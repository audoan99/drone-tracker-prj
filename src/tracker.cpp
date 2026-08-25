// src/tracker.cpp
#include "tracker.h"

GstElement* create_tracker(const gchar *config_file)
{
    GstElement *tracker = gst_element_factory_make("nvtracker", "tracker");
    if (!tracker)
    {
        g_printerr("Failed to create nvtracker.\n");
        return NULL;
    }
    
    g_object_set(G_OBJECT(tracker),
                 "ll-lib-file", "/opt/nvidia/deepstream/deepstream/lib/libnvds_nvmultiobjecttracker.so",
                 "ll-config-file", config_file,
                 "tracker-width", 640,
                 "tracker-height", 384,
                 "display-tracking-id", 1,
                 NULL);
    return tracker;
}