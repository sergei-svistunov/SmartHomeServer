#ifndef VOICECONTROL_H_
#define VOICECONTROL_H_

#include <gst/gst.h>

class VoiceControl {
public:
    VoiceControl();
    virtual ~VoiceControl();
private:
    GstElement *_pipeline, *_source, *_sink;
};

#endif /* VOICECONTROL_H_ */
