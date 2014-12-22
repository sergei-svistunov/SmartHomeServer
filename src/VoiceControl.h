#ifndef VOICECONTROL_H_
#define VOICECONTROL_H_

#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include "libspotter/spotter.h"

class VoiceControl {
public:
    VoiceControl();
    virtual ~VoiceControl();
    GstBuffer *buffer;
    spotter_conf_t *spotter_conf;
    spotter_t *spotter;
private:
    GstElement *_pipeline, *_source, *_convert, *_wavenc, *_sink;
    GstCaps *caps;
    GstAppSinkCallbacks gstCallbacks;
};

#endif /* VOICECONTROL_H_ */
