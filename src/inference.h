// src/inference.h
#ifndef INFERENCE_H
#define INFERENCE_H

#include <gst/gst.h>

GstElement* create_pgie(const gchar *config_file);

#endif // INFERENCE_H