//
// Created by 贺江 on 2018/4/16.
//

#ifndef ATHENAAPP_RTMP_FF_INPUT_H
#define ATHENAAPP_RTMP_FF_INPUT_H

extern "C"
{
#include <libavformat/avformat.h>
#include <libavutil/time.h>
};

#include <QSize>
#include <QThread>
#include <memory>

class InputThread : public QThread
{
Q_OBJECT
public:
    InputThread(QObject *parent);

    virtual ~InputThread();

    void setPath(const char *path);

    signals:

    void onFrame(std::shared_ptr<AVFrame> frame,const AVMediaType &type);


protected:

    void run() override;

    int openRTMP();

    int openCodec(AVMediaType type);

    AVMediaType readFrame(AVFrame *frame);

    double synchronize(AVFrame *srcFrame, double pts);

    QSize getRenderSize(int width,int height);

private:

    char                    path[512];

    AVFormatContext         *formatContext;

    AVInputFormat           *inputFormat;

    AVCodecContext          *videoCodecContext;

    AVCodecContext          *audioCodecContext;

    AVCodec                 *videoCodec;

    AVCodec                 *audioCodec;

    AVStream                *videoStream;

    AVStream                *audioStream;

    AVPacket                *packet;

    AVDictionary            *options;

    int                     audioIndex;

    int                     videoIndex;

    double                  videoClock;
};


#endif //ATHENAAPP_RTMP_FF_INPUT_H
