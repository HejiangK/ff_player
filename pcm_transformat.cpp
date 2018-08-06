//
// Created by 贺江 on 2018/4/23.
//

#include "pcm_transformat.h"


PCMTransFormat::PCMTransFormat()
{
    swrContext  = nullptr;
    buffer      = nullptr;
    fifo        = nullptr;
    maxOutSamples  = 0;
}

PCMTransFormat::~PCMTransFormat()
{
    if(swrContext != nullptr)
    {
        swr_free(&swrContext);

        swrContext = nullptr;
    }

    av_audio_fifo_free(fifo);

    av_freep(&buffer[0]);
    av_freep(&buffer);
}

bool PCMTransFormat::create()
{
    if(swrContext == nullptr)
    {
        swrContext = swr_alloc();

        av_opt_set_int(swrContext, "in_channel_layout",    av_get_default_channel_layout(inChannels), 0);
        av_opt_set_int(swrContext, "in_sample_rate",       inSampleRate, 0);
        av_opt_set_sample_fmt(swrContext, "in_sample_fmt", inSampleFormat, 0);

        av_opt_set_int(swrContext, "out_channel_layout",    av_get_default_channel_layout(outChannels), 0);
        av_opt_set_int(swrContext, "out_sample_rate",       outSampleRate, 0);
        av_opt_set_sample_fmt(swrContext, "out_sample_fmt", outSampleFormat, 0);

        if(swr_init(swrContext) < 0)
        {
            swr_free(&swrContext);
            return false;
        }
    }


    return true;

}

bool PCMTransFormat::trans(AVFrame *frame)
{

    curPts = frame->pts + (double)1 / frame->sample_rate * frame->nb_samples * AV_TIME_BASE;

    int ret;

    if(maxOutSamples == 0)
    {
        inSamples       = frame->nb_samples;
        maxOutSamples   = av_rescale_rnd(inSamples, outSampleRate, inSampleRate, AV_ROUND_UP);

        if(av_samples_alloc_array_and_samples(&buffer, outLineSize, outChannels,
                                              maxOutSamples, outSampleFormat, 0) < 0)
            return false;
    }

    int64_t dstSamples = av_rescale_rnd(swr_get_delay(swrContext, frame->sample_rate) +
                                        frame->nb_samples, outSampleRate, frame->sample_rate, AV_ROUND_UP);

    if(dstSamples > maxOutSamples)
    {
        av_freep(&buffer[0]);
        av_freep(&buffer);

        maxOutSamples = dstSamples;

        if(av_samples_alloc_array_and_samples(&buffer, outLineSize, outChannels,
                                              dstSamples, outSampleFormat, 0) < 0)
            return false;

    }

    if((ret = swr_convert(swrContext, buffer,
                          dstSamples, (const uint8_t **) &frame->data, frame->nb_samples)) < 0)
        return false;

//    int size = av_samples_get_buffer_size(outLineSize, outChannels, ret, outSampleFormat, 1);

    if(fifo == nullptr)
        fifo = av_audio_fifo_alloc(outSampleFormat, outChannels, 1);

    if (av_audio_fifo_write(fifo, (void **) buffer, ret) < ret)
        return false;

    return true;
}

AVFrame *PCMTransFormat::readFrame(int samples)
{
    AVFrame *frame = nullptr;

    int size = av_audio_fifo_size(fifo);

    if (size >= samples)
    {
        frame = av_frame_alloc();

        frame->nb_samples       = samples;
        frame->channel_layout   = (uint64_t)av_get_default_channel_layout(outChannels);
        frame->format           = outSampleFormat;
        frame->sample_rate      = outSampleRate;
        frame->pts              = curPts - (double)1 / frame->sample_rate * (size - samples) * AV_TIME_BASE;

        if (av_frame_get_buffer(frame, 0) < 0)
        {
            av_frame_unref(frame);
            av_frame_free(&frame);

            return nullptr;
        }

        if (av_audio_fifo_read(fifo, (void **) frame->data, samples) < samples)
        {
            av_frame_unref(frame);
            av_frame_free(&frame);

            return nullptr;
        }
    }

    return frame;
}

int PCMTransFormat::getInSampleRate() const
{
    return inSampleRate;
}

int PCMTransFormat::getInChannels() const
{
    return inChannels;
}

AVSampleFormat PCMTransFormat::getInSampleFormat() const
{
    return inSampleFormat;
}

int PCMTransFormat::getOutSampleRate() const
{
    return outSampleRate;
}

int PCMTransFormat::getOutChannels() const
{
    return outChannels;
}

AVSampleFormat PCMTransFormat::getOutSampleFormat() const
{
    return outSampleFormat;
}

void PCMTransFormat::setInSampleRate(int inSampleRate)
{
    PCMTransFormat::inSampleRate = inSampleRate;
}

void PCMTransFormat::setInChannels(int inChannels)
{
    PCMTransFormat::inChannels = inChannels;
}

void PCMTransFormat::setInSampleFormat(AVSampleFormat inSampleFormat)
{
    PCMTransFormat::inSampleFormat = inSampleFormat;
}

void PCMTransFormat::setOutSampleRate(int outSampleRate)
{
    PCMTransFormat::outSampleRate = outSampleRate;
}

void PCMTransFormat::setOutChannels(int outChannels)
{
    PCMTransFormat::outChannels = outChannels;
}

void PCMTransFormat::setOutSampleFormat(AVSampleFormat outSampleFormat)
{
    PCMTransFormat::outSampleFormat = outSampleFormat;
}

