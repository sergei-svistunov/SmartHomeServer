#include "VoiceControl.h"

VoiceControl::VoiceControl() :
        _pipeline(gst_pipeline_new("SmartHome Server")), _source(
                gst_element_factory_make("autoaudiosrc", "microphone")), _sink(
                gst_element_factory_make("autoaudiosink", "audio-output")) {

    gst_bin_add_many(GST_BIN(_pipeline), _source, _sink, NULL);
    gst_element_link(_source, _sink);
}

VoiceControl::~VoiceControl() {
    gst_object_unref(GST_OBJECT(_sink));
    gst_object_unref(GST_OBJECT(_source));
    gst_object_unref(GST_OBJECT(_pipeline));
}
