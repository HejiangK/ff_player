//
// Created by ken on 2018/8/6.
//

#include "video_player_thread.h"
#include "openal_audio_render.h"

VideoPlayerThread::VideoPlayerThread(QObject *parent) : QThread(parent),inputThread(nullptr)
{

}

VideoPlayerThread::~VideoPlayerThread()
{

}

void VideoPlayerThread::setAudioRender(OpenALAudioRender *audioRender)
{
    this->audioRender = audioRender;
}

void VideoPlayerThread::play(const QString &path)
{
    inputThread = new InputThread(this);

    inputThread->setPath(path.toStdString().data());

    inputThread->start();

    start();

    connect(inputThread,&InputThread::onFrame,this,&VideoPlayerThread::onFrame);


}

void VideoPlayerThread::run()
{

    std::shared_ptr<AVFrame> frame;

    int64_t realTime = 0,delay = 0,diff,realDelay,startTime = 0,startPts = 0,lastRealTime = 0;

    while (!isInterruptionRequested())
    {
        frame = videoFrameQueue.front(true);

        if(frame == nullptr)
            break;

        if(startPts == 0)
            startPts = frame->pts;
        if(startTime == 0)
            startTime = av_gettime();

        if(audioRender != nullptr)
        {
            realTime = audioRender->getRealTime();
            diff     = realTime < 0 ? 0 : frame->pts - realTime;
        } else
        {
            realTime    = av_gettime() - startTime;
            diff        = frame->pts - startPts - realTime;
        }

        if(videoFrameQueue.length() > 0)
            delay = videoFrameQueue.at(0)->pts - frame->pts;
        else
            delay = delay == 0 ? frame->pkt_duration : delay;

        if(delay <= 0 || delay > 1000000)
        {
            delay = AV_TIME_BASE / 30;
        }

        if(qAbs(diff) > 100000)
        {
            if(diff < 0)
            {
                if(qAbs(diff) > 300000)
                    realDelay = 0;
                else
                    realDelay = delay / 2;
            } else
            {
                if(qAbs(diff) > 300000)
                    realDelay = delay * 2;
                else
                    realDelay = delay * 1.5;
            }

        } else
        {
            realDelay = delay;
        }

        if(realDelay > 0)
            usleep(realDelay);

        emit onVideoFrame(frame);

    }

    inputThread->requestInterruption();

    inputThread->wait();

    inputThread->deleteLater();

}

void VideoPlayerThread::onFrame(const std::shared_ptr<AVFrame> &frame, const AVMediaType &type)
{
    if(type == AVMEDIA_TYPE_AUDIO)
        emit onAudioFrame(frame);
    else
        videoFrameQueue.push(frame);
}


