//
// Created by 贺江 on 2018/4/23.
//

#include "image_transformat.h"

ImageTransformat::ImageTransformat()
{

}

ImageTransformat::~ImageTransformat()
{
    if(swsContext != nullptr)
    {
        sws_freeContext(swsContext);

        swsContext = nullptr;
    }


    if(yuvFrame != nullptr)
    {
        av_freep(&yuvFrame->data[0]);
        av_frame_unref(yuvFrame);
        av_frame_free(&yuvFrame);

        yuvFrame = nullptr;
    }

}

void ImageTransformat::create()
{
    this->swsContext        = sws_getContext(inWidth, inHeight, inPixelFormat,
                                             outWidth, outHeight, outPixelFormat,
                                             SWS_AREA, nullptr, nullptr, nullptr);

    this->yuvFrame		    = av_frame_alloc();

    av_image_alloc(yuvFrame->data,yuvFrame->linesize,outWidth,outHeight,outPixelFormat,1);
}

AVFrame *ImageTransformat::trans(AVFrame *frame)
{
    sws_scale(this->swsContext, (const uint8_t *const *) frame->data, frame->linesize, 0,
              this->inHeight, this->yuvFrame->data, this->yuvFrame->linesize);

    this->yuvFrame->width           = this->outWidth;
    this->yuvFrame->height          = this->outHeight;
    this->yuvFrame->format          = this->outPixelFormat;
    this->yuvFrame->pts	            = frame->pts;
    this->yuvFrame->pkt_duration    = frame->pkt_duration;

    return yuvFrame;
}

AVPixelFormat ImageTransformat::getInPixelFormat() const
{
    return inPixelFormat;
}

AVPixelFormat ImageTransformat::getOutPixelFormat() const
{
    return outPixelFormat;
}

int ImageTransformat::getInWidth() const
{
    return inWidth;
}

int ImageTransformat::getInHeight() const
{
    return inHeight;
}

int ImageTransformat::getOutWidth() const
{
    return outWidth;
}

int ImageTransformat::getOutHeight() const
{
    return outHeight;
}

void ImageTransformat::setInPixelFormat(AVPixelFormat inPixelFormat)
{
    ImageTransformat::inPixelFormat = inPixelFormat;
}

void ImageTransformat::setOutPixelFormat(AVPixelFormat outPixelFormat)
{
    ImageTransformat::outPixelFormat = outPixelFormat;
}

void ImageTransformat::setInWidth(int inWidth)
{
    ImageTransformat::inWidth = inWidth;
}

void ImageTransformat::setInHeight(int inHeight)
{
    ImageTransformat::inHeight = inHeight;
}

void ImageTransformat::setOutWidth(int outWidth)
{
    ImageTransformat::outWidth = outWidth;
}

void ImageTransformat::setOutHeight(int outHeight)
{
    ImageTransformat::outHeight = outHeight;
}

