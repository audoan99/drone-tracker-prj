// src/tracker.h
#ifndef TRACKER_H
#define TRACKER_H

#include <gst/gst.h>

GstElement* create_tracker(const gchar *config_file);

#endif // TRACKER_H