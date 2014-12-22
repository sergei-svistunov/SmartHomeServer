#include "VoiceControl.h"

#include <iostream>
#include <thread>
#include <exception>

#include <memory.h>

using namespace std;

GstFlowReturn on_new_buffer_from_source(GstAppSink * elt, void * data) {
    GstSample *sample = gst_app_sink_pull_sample(GST_APP_SINK(elt));

    VoiceControl *vc = (VoiceControl*) data;

    if (!vc->buffer)
        vc->buffer = gst_buffer_new();

    gst_buffer_append(vc->buffer, gst_sample_get_buffer(sample));

    auto bufferSize = gst_buffer_get_size(vc->buffer);

    GstMapInfo info;
    if (bufferSize >= 10000) {
        gst_buffer_map(vc->buffer, &info, GST_MAP_READ);

        int w, unhandled;
        if ((w = spotter_feed_data(vc->spotter, info.data, bufferSize, &unhandled)) != 0) {
            cerr << spotter_get_phrase(vc->spotter, w) << endl;
        }

        gst_buffer_unmap(vc->buffer, &info);

        gst_buffer_unref(vc->buffer);
        vc->buffer = NULL;
    }

    return GST_FLOW_OK;
}

VoiceControl::VoiceControl() :
        _pipeline(gst_pipeline_new("SmartHome Server")), _source(
                gst_element_factory_make("autoaudiosrc", "microphone")), _convert(
                gst_element_factory_make("audioconvert", "audioconvertS16")), _wavenc(
                gst_element_factory_make("wavenc", "wave-encoder")), _sink(
                gst_element_factory_make("appsink", "audio-output")), caps(
                gst_caps_new_simple("audio/x-raw", "format", G_TYPE_STRING, "S16LE", "rate", G_TYPE_INT, 16000,
                        "channels", G_TYPE_INT, 1, NULL)), buffer(NULL) {

    spotter_conf = spotter_conf_read(SPOTTER_CONF);
    if (spotter_conf == NULL) {
        throw std::runtime_error("Cannot read spotter config");
    }
    spotter = spotter_create(spotter_conf, 16000);

//    g_object_set(G_OBJECT(_sink), "location", SH_MIC_FN, NULL);

    memset(&gstCallbacks, 0, sizeof(GstAppSinkCallbacks));
    gstCallbacks.new_sample = on_new_buffer_from_source;
    gst_app_sink_set_callbacks(GST_APP_SINK(_sink), &gstCallbacks, this, NULL);

    gst_bin_add_many(GST_BIN(_pipeline), _source, _convert, _wavenc, _sink, NULL);
    gst_element_link(_source, _convert);
    gst_element_link_filtered(_convert, _wavenc, caps);
    gst_element_link(_wavenc, _sink);
    gst_element_set_state(_pipeline, GST_STATE_PLAYING);

    thread voiceThread([this]() {
        GMainLoop *loop = g_main_loop_new(NULL, FALSE);
        g_main_loop_run(loop);

    });
    voiceThread.detach();
}

VoiceControl::~VoiceControl() {
    gst_object_unref(GST_OBJECT(_sink));
    gst_object_unref(GST_OBJECT(_source));
    gst_object_unref(GST_OBJECT(_pipeline));
}
