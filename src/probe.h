#ifndef PROBE_H
#define PROBE_H

#include <gst/gst.h>
// Corrected DeepStream header names (no underscores)
#include "nvdsmeta.h"
#include "gstnvdsmeta.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief GStreamer pad probe callback to extract metadata
 */
GstPadProbeReturn osd_sink_pad_buffer_probe(GstPad *pad, GstPadProbeInfo *info, gpointer u_data);

#ifdef __cplusplus
}
#endif

#endif // PROBE_H