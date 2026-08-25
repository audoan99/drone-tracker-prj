// src/probe.cpp
#include "probe.h"
#include <stdio.h>

GstPadProbeReturn osd_sink_pad_buffer_probe(GstPad *pad, GstPadProbeInfo *info, gpointer u_data) 
{
    GstBuffer *buf = (GstBuffer *)info->data;
    NvDsBatchMeta *batch_meta = gst_buffer_get_nvds_batch_meta(buf);

    if (!batch_meta) return GST_PAD_PROBE_OK;

    for (NvDsMetaList *l_frame = batch_meta->frame_meta_list; l_frame != NULL; l_frame = l_frame->next)
    {
        NvDsFrameMeta *frame_meta = (NvDsFrameMeta *)(l_frame->data);

        for (NvDsMetaList *l_obj = frame_meta->obj_meta_list; l_obj != NULL; l_obj = l_obj->next)
        {
            NvDsObjectMeta *obj_meta = (NvDsObjectMeta *)(l_obj->data);
            printf("[Drone Tracker] Frame: %lu | ID: %ld | Class: %d | Conf: %.2f\n",
                   frame_meta->frame_num, obj_meta->object_id, obj_meta->class_id, obj_meta->confidence);
        }
    }
    return GST_PAD_PROBE_OK;
}