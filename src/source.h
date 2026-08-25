// src/source.h
#ifndef SOURCE_H
#define SOURCE_H

#include <gst/gst.h>

// Creates a source bin and links it to the provided streammux element
GstElement* create_source_bin(guint index, const gchar *uri, GstElement *streammux);

#endif // SOURCE_H