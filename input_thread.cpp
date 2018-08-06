//
// Created by 贺江 on 2018/4/16.
//

#include "input_thread.h"
#include "pcm_transformat.h"
#include "image_transformat.h"

InputThread::InputThread(QObject *parent):QThread(parent)
{
    this->inputFormat       = nullptr;
    this->audioCodecContext = nullptr;
    this->videoCodecContext = nullptr;
    this->audioStream       = nullptr;
    this->videoStream       = nullptr;
    this->audioCodec        = nullptr;
    this->videoCodec        = nullptr;
    this->options           = nullptr;
    this->formatContext     = nullptr;

    this->videoClock        = 0;
    this->audioIndex        = -1;
    this->videoIndex        = -1;
    this->packet            = av_packet_alloc();
}

InputThread::~InputThread()
{

}

void InputThread::setPath(const char *path)
{
    sprintf(this->path,"%s",path);
}

void InputThread::run()
{
    if(openRTMP() != 0)
        return;

    AVFrame     *frame,*transFrame;
    AVMediaType type;

    PCMTransFormat   *pcmTransFormat    = nullptr;
    ImageTransformat *imageTransformat  = nullptr;

    while (!isInterruptionRequested())
    {
        frame = av_frame_alloc();

        type = readFrame(frame);

        if(type == AVMEDIA_TYPE_UNKNOWN)
        {
            av_frame_unref(frame);
            av_frame_free(&frame);
            continue;
        }

        if(type == AVMEDIA_TYPE_AUDIO)
        {
            if(pcmTransFormat == nullptr)
            {
                pcmTransFormat = new PCMTransFormat();

                pcmTransFormat->setInChannels(audioCodecContext->channels);
                pcmTransFormat->setInSampleFormat(audioCodecContext->sample_fmt);
                pcmTransFormat->setInSampleRate(audioCodecContext->sample_rate);

                pcmTransFormat->setOutChannels(2);
                pcmTransFormat->setOutSampleFormat(AV_SAMPLE_FMT_S16);
                pcmTransFormat->setOutSampleRate(44100);

                pcmTransFormat->create();
            }

            if(!pcmTransFormat->trans(frame))
                break;

            transFrame = pcmTransFormat->readFrame(1024);

            if(transFrame != nullptr)
            {

                std::shared_ptr<AVFrame> shFrame = std::shared_ptr<AVFrame>(frame,[](AVFrame *temp)->void{

                    av_free(temp->data[0]);

                    av_frame_unref(temp);
                    av_frame_free(&temp);

                });

                shFrame->sample_rate      = transFrame->sample_rate;
                shFrame->channels         = transFrame->channels;
                shFrame->format           = transFrame->format;
                shFrame->pts              = transFrame->pts;
                shFrame->linesize[0]      = transFrame->linesize[0];
                shFrame->data[0]          = (uint8_t *)av_malloc(transFrame->linesize[0]);

                memcpy(shFrame->data[0],transFrame->data[0],transFrame->linesize[0]);

                av_frame_unref(transFrame);
                av_frame_free(&transFrame);

                emit onFrame(shFrame,type);

            }

        } else if(type == AVMEDIA_TYPE_VIDEO)
        {
            if(imageTransformat == nullptr)
            {
                QSize size = getRenderSize(frame->width,frame->height);

                imageTransformat = new ImageTransformat();

                imageTransformat->setInPixelFormat((AVPixelFormat)frame->format);
                imageTransformat->setInWidth(frame->width);
                imageTransformat->setInHeight(frame->height);

                imageTransformat->setOutPixelFormat(AV_PIX_FMT_YUV420P);
                imageTransformat->setOutWidth(size.width());
                imageTransformat->setOutHeight(size.height());

                imageTransformat->create();
            }

            transFrame = imageTransformat->trans(frame);

            emit onFrame(std::shared_ptr<AVFrame>(av_frame_clone(transFrame),[](AVFrame *temp)->void{

                av_frame_unref(temp);
                av_frame_free(&temp);

            }),type);

        }


    }

   // avio_close(formatContext->pb);
    avformat_close_input(&formatContext);

    avcodec_close(videoCodecContext);
    avcodec_close(audioCodecContext);

    avcodec_free_context(&videoCodecContext);
    avcodec_free_context(&audioCodecContext);

    delete imageTransformat;
    delete pcmTransFormat;
}

QSize InputThread::getRenderSize(int width,int height)
{
    int w = width,h;

    double scale = (double)width / height;

    while (true)
    {
        if(w == FFALIGN(w,32) && w / 2 == FFALIGN(w / 2,32))
        {
            break;
        }

        w ++;
    }

    h = w / scale;

    return QSize(w,h);
}

int InputThread::openRTMP()
{
    this->formatContext = avformat_alloc_context();

//    this->formatContext->interrupt_callback.callback = onStreamTimeOut;
//    this->formatContext->interrupt_callback.opaque   = this;

    if(avformat_open_input(&this->formatContext,path,this->inputFormat,&options) != 0)
        return -1;
    if(avformat_find_stream_info(this->formatContext, nullptr) != 0)
        return -1;

    openCodec(AVMEDIA_TYPE_VIDEO);

    openCodec(AVMEDIA_TYPE_AUDIO);

    return 0;

}

int InputThread::openCodec(AVMediaType type)
{
    AVCodecContext *codecContext    = nullptr;
    AVCodec        *codec           = nullptr;
    AVStream       *stream          = nullptr;
    AVCodecID      codecID;
    for(int i = 0; i < this->formatContext->nb_streams; i ++)
    {
        if(this->formatContext->streams[i]->codecpar->codec_type == type)
        {
            codecContext = avcodec_alloc_context3(nullptr);
            stream       = this->formatContext->streams[i];
            codecID      = stream->codecpar->codec_id;

            avcodec_parameters_to_context(codecContext, stream->codecpar);
            av_codec_set_pkt_timebase(codecContext, stream->time_base);

            codec = avcodec_find_decoder(codecID);

            if(type == AVMEDIA_TYPE_VIDEO)
            {
                this->videoIndex        = i;
                this->videoCodecContext = codecContext;
                this->videoCodec        = codec;
                this->videoStream       = stream;
            }else if(type == AVMEDIA_TYPE_AUDIO)
            {
                this->audioIndex        = i;
                this->audioCodecContext = codecContext;
                this->audioCodec        = codec;
                this->audioStream       = stream;
            }

            if(codec == nullptr)
                return -1;
            if(avcodec_open2(codecContext,codec,nullptr) != 0)
                return -1;
            break;
        }
    }
    return 0;
}

AVMediaType InputThread::readFrame(AVFrame *frame)
{
    AVCodecContext  *codecContext;
    double          vpts;
    int             ret;
    AVMediaType     type;

    if(av_read_frame(formatContext,packet) < 0)
        return AVMEDIA_TYPE_UNKNOWN;

    codecContext = packet->stream_index == audioIndex ? audioCodecContext : videoCodecContext;
    type         = codecContext->codec_type;

    if((ret = avcodec_send_packet(codecContext,packet)) != 0)
    {
        if(ret == AVERROR(EAGAIN))
            return readFrame(frame);
        else
            return AVMEDIA_TYPE_UNKNOWN;
    }

    if((ret = avcodec_receive_frame(codecContext,frame)) != 0)
    {

        if(ret == AVERROR(EAGAIN))
        {
            return readFrame(frame);

        } else
            return AVMEDIA_TYPE_UNKNOWN;
    }


    if(type == AVMEDIA_TYPE_VIDEO)
    {

        if ((vpts = av_frame_get_best_effort_timestamp(frame)) == AV_NOPTS_VALUE)
            vpts = 0;

        vpts *= av_q2d(videoStream->time_base);

        frame->pts          = synchronize(frame, vpts) * 1000000;
        frame->pkt_duration = AV_TIME_BASE * videoStream->r_frame_rate.den / videoStream->r_frame_rate.num;
    } else
    {

        if (packet->pts != AV_NOPTS_VALUE)
            frame->pts   = av_q2d(audioStream->time_base) * packet->pts * 1000000;
        else
            frame->pts   = 0;

        frame->sample_rate      = audioCodecContext->sample_rate;
        frame->channels         = audioCodecContext->channels;
        frame->format           = *audioCodec->sample_fmts;
        frame->pts              = frame->pts;
        frame->pkt_duration     = AV_TIME_BASE * (1 / av_q2d({audioCodecContext->sample_rate, 1})) * frame->nb_samples;

    }

    av_packet_unref(packet);

    return type;
}

double InputThread::synchronize(AVFrame *srcFrame, double pts)
{
    double frame_delay;

    if (pts != 0)
        videoClock = pts;
    else
        pts = videoClock;

    frame_delay = av_q2d(videoStream->time_base);
    frame_delay += srcFrame->repeat_pict * (frame_delay * 0.5);

    videoClock += frame_delay;

    return pts;
}
