// src/pipeline.h
#ifndef PIPELINE_H
#define PIPELINE_H

#include <gst/gst.h>
#include "config_parser.h"

// Builds the complete DeepStream pipeline
GstElement* build_pipeline(const PipelineConfig& config);

#endif // PIPELINE_H
