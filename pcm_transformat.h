//
// Created by 贺江 on 2018/4/23.
//

#ifndef ATHENAAPP_PCM_TRANSFORMAT_H
#define ATHENAAPP_PCM_TRANSFORMAT_H


extern "C"
{
#include <libavutil/time.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
#include <libavutil/audio_fifo.h>
};

class PCMTransFormat
{

public:
    PCMTransFormat();

    virtual ~PCMTransFormat();

    int getInSampleRate() const;

    int getInChannels() const;

    AVSampleFormat getInSampleFormat() const;

    int getOutSampleRate() const;

    int getOutChannels() const;

    AVSampleFormat getOutSampleFormat() const;

    void setInSampleRate(int inSampleRate);

    void setInChannels(int inChannels);

    void setInSampleFormat(AVSampleFormat inSampleFormat);

    void setOutSampleRate(int outSampleRate);

    void setOutChannels(int outChannels);

    void setOutSampleFormat(AVSampleFormat outSampleFormat);

    bool create();

    bool trans(AVFrame *frame);

    AVFrame *readFrame(int samples);

private:

    SwrContext      *swrContext;

    uint8_t         **buffer;

    int             inSampleRate;

    int             inChannels;

    int             inSamples;

    AVSampleFormat  inSampleFormat;

    int             outSampleRate;

    int             outChannels;

    int64_t         maxOutSamples;

    AVSampleFormat  outSampleFormat;

    int             outLineSize[8];

    AVAudioFifo     *fifo;

    int64_t         curPts;
};




#endif //ATHENAAPP_PCM_TRANSFORMAT_H
