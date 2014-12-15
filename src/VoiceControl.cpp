#include "VoiceControl.h"

#include <thread>

using namespace std;

VoiceControl::VoiceControl() :
        _pipeline(gst_pipeline_new("SmartHome Server")), _source(
                gst_element_factory_make("autoaudiosrc", "microphone")), _wavenc(
                gst_element_factory_make("wavenc", "wave-encoder")), _sink(
                gst_element_factory_make("filesink", "audio-output")) {

    g_object_set(G_OBJECT(_sink), "location", "/tmp/123.raw", NULL);

    gst_bin_add_many(GST_BIN(_pipeline), _source, _wavenc, _sink, NULL);
    gst_element_link(_source, _wavenc);
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
