//
// Created by ken on 2018/8/6.
//

#ifndef FF_PLAYER_VIDEO_PLAYER_H
#define FF_PLAYER_VIDEO_PLAYER_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QEvent>

extern "C"
{
#include <libavformat/avformat.h>
};

class OpenGLVideoRender;
class OpenALAudioRender;
class VideoPlayerThread;

class VideoPlayer: public QWidget
{
public:
    VideoPlayer();

    virtual ~VideoPlayer();

    bool eventFilter(QObject *watched, QEvent *event) override;

protected:

    void setupView();

    void play();

    void stop();

private:

    OpenGLVideoRender   *videoRender;

    OpenALAudioRender   *audioRender;

    VideoPlayerThread   *videoThread;

    QVBoxLayout         *mainLayout;

    QWidget             *toolBox;

    QHBoxLayout         *toolLayout;

    QPushButton         *btnPlay;

    QPushButton         *btnStop;

    QTextEdit           *inputText;

};


#endif //FF_PLAYER_VIDEO_PLAYER_H
