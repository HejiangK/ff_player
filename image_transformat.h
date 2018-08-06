//
// Created by 贺江 on 2018/4/23.
//

#ifndef ATHENAAPP_IMAGE_TRANSFORMAT_H
#define ATHENAAPP_IMAGE_TRANSFORMAT_H

extern "C"
{
#include <libavutil/time.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavutil/audio_fifo.h>
#include <libavutil/imgutils.h>
};

class  ImageTransformat
{
public:
    ImageTransformat();

    virtual ~ImageTransformat();

    AVPixelFormat getInPixelFormat() const;

    AVPixelFormat getOutPixelFormat() const;

    int getInWidth() const;

    int getInHeight() const;

    int getOutWidth() const;

    int getOutHeight() const;

    void setInPixelFormat(AVPixelFormat inPixelFormat);

    void setOutPixelFormat(AVPixelFormat outPixelFormat);

    void setInWidth(int inWidth);

    void setInHeight(int inHeight);

    void setOutWidth(int outWidth);

    void setOutHeight(int outHeight);

    void create();

    AVFrame *trans(AVFrame *frame);

private:

    AVPixelFormat   inPixelFormat;

    AVPixelFormat   outPixelFormat;

    int             inWidth;

    int             inHeight;

    int             outWidth;

    int             outHeight;

    SwsContext      *swsContext;

    AVFrame         *yuvFrame;

};


#endif //ATHENAAPP_IMAGE_TRANSFORMAT_H
