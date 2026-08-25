#include "sink.h"

GstElement* create_sink_bin(int port) {
    GstElement *sink_bin = gst_bin_new("sink-bin");

    // 1. Video Convert
    // Thử dùng nvvideoconvert, nếu không có thì dùng videoconvert chuẩn
    GstElement *vidconv = gst_element_factory_make("nvvideoconvert", "sink-nvvidconv");
    gboolean use_nvmm = TRUE;
    if (!vidconv) {
        g_print("nvvideoconvert not found, falling back to standard videoconvert...\n");
        vidconv = gst_element_factory_make("videoconvert", "sink-videoconvert");
        use_nvmm = FALSE;
    }

    GstElement *caps_filter = gst_element_factory_make("capsfilter", "sink-caps-filter");

    // 2. Encoder (Logic mới: Thử nvh264enc trước, fallback x264enc)
    GstElement *encoder = gst_element_factory_make("nvh264enc", "sink-nvh264-encoder");
    gboolean is_hw_encoder = FALSE;
    
    if (encoder) {
        g_print("Using NVIDIA NVENC (nvh264enc) for hardware encoding.\n");
        is_hw_encoder = TRUE;
    } else {
        g_print("nvh264enc not found, falling back to x264enc (CPU encoding)...\n");
        encoder = gst_element_factory_make("x264enc", "sink-x264-encoder");
    }

    GstElement *parser = gst_element_factory_make("h264parse", "sink-h264-parser");
    GstElement *payloader = gst_element_factory_make("rtph264pay", "sink-rtp-payloader");
    GstElement *udp_sink = gst_element_factory_make("udpsink", "sink-udp");

    if (!vidconv || !caps_filter || !encoder || !parser || !payloader || !udp_sink) {
        g_printerr("Failed to create sink bin elements.\n");
        return NULL;
    }

    // 3. Cấu hình Caps
    // nvh264enc và x264enc đều yêu cầu đầu vào là RAM thường (không phải NVMM)
    // Nếu vidconv là nvvideoconvert, ta cần ép nó xuất ra RAM thường bằng cách set caps I420 hoặc NV12 ở RAM
    GstCaps *caps;
    if (use_nvmm) {
        // Nếu đang dùng NVMM, ta cần một bước chuyển đổi xuống RAM thường cho Encoder
        // Cách đơn giản nhất là set caps filter ở đầu ra của vidconv thành RAM thường
        caps = gst_caps_from_string("video/x-raw, format=NV12"); 
    } else {
        caps = gst_caps_from_string("video/x-raw, format=NV12");
    }
    g_object_set(G_OBJECT(caps_filter), "caps", caps, NULL);
    gst_caps_unref(caps);

    // 4. Encoder
    if (is_hw_encoder) {
        // nvh264enc
        g_object_set(G_OBJECT(encoder), "bitrate", 4000, NULL);
    } else {
        // x264enc (CPU) (ultrafast, zerolatency)
        g_object_set(G_OBJECT(encoder), "bitrate", 4000, "speed-preset", 1, "tune", 0x00000004, NULL);
    }

    // 5. Configure UDP Sink
    g_object_set(G_OBJECT(udp_sink), "host", "127.0.0.1", "port", port, "sync", 0, "async", FALSE, NULL);

    // 6. Add bin and link
    gst_bin_add_many(GST_BIN(sink_bin), vidconv, caps_filter, encoder, parser, payloader, udp_sink, NULL);
    
    if (!gst_element_link_many(vidconv, caps_filter, encoder, parser, payloader, udp_sink, NULL)) {
        g_printerr("Failed to link sink bin elements.\n");
        return NULL;
    }

    // 7. Create ghost pad
    GstPad *pad = gst_element_get_static_pad(vidconv, "sink");
    gst_element_add_pad(sink_bin, gst_ghost_pad_new("sink", pad));
    gst_object_unref(pad);

    g_print("Sink bin created successfully using UDP streaming on port %d\n", port);
    return sink_bin;
}