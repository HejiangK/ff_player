//
// Created by ken on 2018/8/6.
//

#include "video_player.h"
#include "opengl_video_render.h"
#include "openal_audio_render.h"
#include "video_player_thread.h"
#include "input_thread.h"

VideoPlayer::VideoPlayer()
{
    videoRender     = nullptr;
    audioRender     = nullptr;
    videoThread     = nullptr;

    audioRender = new OpenALAudioRender();

    audioRender->setFormat(AV_SAMPLE_FMT_S16);
    audioRender->setSampleRate(44100);
    audioRender->setChannels(2);
    audioRender->open();

    setupView();
}

VideoPlayer::~VideoPlayer()
{

}

void VideoPlayer::setupView()
{
    mainLayout = new QVBoxLayout(this);

    mainLayout->setSpacing(5);

    toolBox = new QWidget(this);

    toolBox->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    toolBox->setFixedHeight(40);

    videoRender = new OpenGLVideoRender(this);

    mainLayout->addWidget(videoRender);
    mainLayout->addWidget(toolBox);

    toolLayout = new QHBoxLayout(toolBox);

    toolLayout->setSpacing(5);
    toolLayout->setContentsMargins(0,0,0,0);

    inputText = new QTextEdit(toolBox);

    inputText->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    inputText->setText("rtmp://live.hkstv.hk.lxdns.com/live/hks");

    btnPlay = new QPushButton(toolBox);

    btnPlay->setText(u8"播放");
    btnPlay->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);


    btnStop = new QPushButton(toolBox);

    btnStop->setText(u8"停止");
    btnStop->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);


    toolLayout->addWidget(inputText);
    toolLayout->addWidget(btnPlay);
    toolLayout->addWidget(btnStop);


    btnStop->installEventFilter(this);
    btnPlay->installEventFilter(this);
}

bool VideoPlayer::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress)
    {

        if(watched == btnPlay)
            play();
        else
            stop();

    }

    return QObject::eventFilter(watched, event);
}

void VideoPlayer::play()
{

    if(videoThread == nullptr)
    {
        videoThread = new VideoPlayerThread(this);

        videoThread->setAudioRender(audioRender);
        videoThread->play(inputText->toPlainText());

        connect(videoThread,&VideoPlayerThread::onVideoFrame,videoRender,&OpenGLVideoRender::onRenderFrame);
        connect(videoThread,&VideoPlayerThread::onAudioFrame,audioRender,&OpenALAudioRender::onRenderFrame);

    }

}

void VideoPlayer::stop()
{
    if(videoThread != nullptr)
    {
        videoThread->requestInterruption();

        connect(videoThread,&QThread::finished,[this]()->void{

            videoThread->deleteLater();

            videoThread = nullptr;

        });
    }
}



