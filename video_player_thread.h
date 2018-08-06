//
// Created by ken on 2018/8/6.
//

#ifndef FF_PLAYER_RENDER_THREAD_H
#define FF_PLAYER_RENDER_THREAD_H

#include <QThread>
#include <memory>

#include "input_thread.h"
#include "sync_queue.h"

class OpenALAudioRender;

class VideoPlayerThread : public QThread
{
    Q_OBJECT
public:
    VideoPlayerThread(QObject *parent);

    virtual ~VideoPlayerThread();

    void play(const QString &path);

    void setAudioRender(OpenALAudioRender *audioRender);

    signals:

    void onVideoFrame(const std::shared_ptr<AVFrame> &frame);

    void onAudioFrame(const std::shared_ptr<AVFrame> &frame);

protected:

    void run() override;

    void onFrame(const std::shared_ptr<AVFrame> &frame,const AVMediaType &type);


private:

    InputThread         *inputThread;

    OpenALAudioRender   *audioRender;

    KSyncQueue<std::shared_ptr<AVFrame>>    videoFrameQueue;
};


#endif //FF_PLAYER_RENDER_THREAD_H
