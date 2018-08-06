//
// Created by Ken on 2018/5/16.
//

#ifndef ATHENAAPP_OPENAL_AUDIO_RENDER_H
#define ATHENAAPP_OPENAL_AUDIO_RENDER_H

#include <al.h>
#include <alc.h>
#include <QThread>
#include <memory>
#include <QList>
#include <QThread>

#include "sync_queue.h"

#define NUM_BUFFERS 3

extern "C"
{
#include <libavformat/avformat.h>
};


class OpenALAudioRender : public QThread
{
public:

    OpenALAudioRender();

    virtual ~OpenALAudioRender();

    void setSampleRate(int sampleRate);

    void setChannels(int channels);

    void setFormat(AVSampleFormat format);

    void open();

    void close() ;

    int64_t getRealTime();

    void onRenderFrame(const std::shared_ptr<AVFrame> &frame);

protected:

    void run() override;

    bool createALDevie();

    bool initALSource();

    bool fillALSource();

    void removeALSource();

private:

    using SHFrame = std::shared_ptr<AVFrame> ;

    KSyncQueue<SHFrame>    frameQueue;

    QMutex          mutex;
    int64_t         curPts;
    int             sampleRate;
    int             channels;
    AVSampleFormat  format;
    ALuint          source;
    ALuint          buffers[NUM_BUFFERS];

};



#endif //ATHENAAPP_OPENAL_AUDIO_RENDER_H
