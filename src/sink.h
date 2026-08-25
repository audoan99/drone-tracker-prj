// src/sink.h
#ifndef SINK_H
#define SINK_H

#include <gst/gst.h>

// Creates the encoding chain and RTSP sink. 
// Returns the first element (nvvideoconvert) to be linked from the pipeline.
GstElement* create_sink_bin(int port);

#endif // SINK_H