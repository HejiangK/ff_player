
#include <QApplication>
#include <QSurfaceFormat>

extern "C"
{
#include <libavformat/avformat.h>
}

#include "video_player.h"

Q_DECLARE_METATYPE(std::shared_ptr<AVFrame>);
Q_DECLARE_METATYPE(AVMediaType);


int main(int argc,char **argv)
{
#ifdef Q_OS_MAC
    QSurfaceFormat format;

    format.setMajorVersion(3);
    format.setMinorVersion(3);

    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);
#else

    QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
#endif

    QApplication app(argc,argv);

    av_register_all();
    avformat_network_init();

    qRegisterMetaType<std::shared_ptr<AVFrame> >();
    qRegisterMetaType<std::shared_ptr<AVFrame> >("std::shared_ptr<AVFrame>const&");

    qRegisterMetaType<AVMediaType>();
    qRegisterMetaType<AVMediaType>("AVMediaType const&");

    VideoPlayer videoPlayer;

    videoPlayer.resize(800,600);

    videoPlayer.show();

    return app.exec();
}