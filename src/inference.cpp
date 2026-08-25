// src/inference.cpp
#include "inference.h"

GstElement* create_pgie(const gchar *config_file)
{
    GstElement *pgie = gst_element_factory_make("nvinfer", "primary-nvinference-engine");
    if (!pgie)
    {
        g_printerr("Failed to create nvinfer (PGIE).\n");
        return NULL;
    }
    g_object_set(G_OBJECT(pgie), "config-file-path", config_file, NULL);
    return pgie;
}
