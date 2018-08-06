//
// Created by Ken on 2018/5/16.
//

#include "openal_audio_render.h"

#include <QDebug>

static ALCdevice       *device = nullptr;

static ALCcontext      *context = nullptr;

static int             deviceRef = 0;

OpenALAudioRender::OpenALAudioRender()
{

}

OpenALAudioRender::~OpenALAudioRender()
{

}

void OpenALAudioRender::open()
{
    start();
}

void OpenALAudioRender::close()
{
    frameQueue.terminal();

    requestInterruption();
}

int64_t OpenALAudioRender::getRealTime()
{
    QMutexLocker locker(&mutex);

    ALint samples = 0;

    alGetSourcei(source, AL_SAMPLE_OFFSET, &samples);

    samples = 1024 * NUM_BUFFERS - samples;

    return curPts - samples * AV_TIME_BASE / sampleRate;
}

void OpenALAudioRender::setSampleRate(int sampleRate)
{
    OpenALAudioRender::sampleRate = sampleRate;
}

void OpenALAudioRender::setChannels(int channels)
{
    OpenALAudioRender::channels = channels;
}

void OpenALAudioRender::setFormat(AVSampleFormat format)
{
    OpenALAudioRender::format = format;
}

void OpenALAudioRender::run()
{

    int  duration = static_cast<int>((float)1024 * 1000 / sampleRate);

    if(!createALDevie())
        return;

    if(!initALSource())
        return;

    while (!isInterruptionRequested())
    {

        if(!fillALSource())
            break;

        msleep(duration);
    }

    removeALSource();
}

bool OpenALAudioRender::createALDevie()
{
    QMutexLocker locker(&mutex);

    if(device == nullptr)
    {
        device  = alcOpenDevice(nullptr);
        context = alcCreateContext(device, nullptr);

        alcMakeContextCurrent(context);

        if (alcGetError(device) != ALC_NO_ERROR)
            return false;
    }

    deviceRef ++;

    alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
    alListener3f(AL_POSITION, 0, 0, 0);

    alGenSources(1,&source); //创建一个播放source

    ALfloat SourcePos[] = { 0.0, 0.0, 0.0 };
    ALfloat SourceVel[] = { 0.0, 0.0, 0.0 };

    alSourcef(source, AL_PITCH, 1.0);
    alSourcef(source, AL_GAIN, 1.0);
    alSourcefv(source, AL_POSITION, SourcePos);
    alSourcefv(source, AL_VELOCITY, SourceVel);
    alSourcef(source, AL_REFERENCE_DISTANCE, 50.0f);
    alSourcei(source, AL_LOOPING, AL_FALSE);

    alGenBuffers(NUM_BUFFERS, buffers);

    return true;
}

bool OpenALAudioRender::initALSource()
{
    QMutexLocker locker(&mutex);

    SHFrame frame;

    curPts = 0;

    for (int i = 0; i < NUM_BUFFERS; i++)
    {
        frame = frameQueue.front(true);

        if(frame == nullptr || isInterruptionRequested())
            return false;

        curPts = frame->pts;

        alBufferData(buffers[i], AL_FORMAT_STEREO16, frame->data[0], frame->linesize[0], sampleRate);
        alSourceQueueBuffers(source, 1, &buffers[i]);
    }

    alSourcePlay(source);

    return true;
}

bool OpenALAudioRender::fillALSource()
{
    QMutexLocker locker(&mutex);

    SHFrame frame;
    ALint   processed = 0;
    ALuint  bufferId = 0;
    int     state;

    alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);

    while (processed > 0)
    {
        alSourceUnqueueBuffers(source, 1, &bufferId);

        frame = frameQueue.front(true);

        if(frame == nullptr)
            return false;

        curPts = frame->pts;

        alBufferData(bufferId, AL_FORMAT_STEREO16, frame->data[0], frame->linesize[0], sampleRate);
        alSourceQueueBuffers(source, 1, &bufferId);

        processed --;
    }

    alGetSourcei(source, AL_SOURCE_STATE, &state);

    if (state == AL_STOPPED || state == AL_INITIAL)
        alSourcePlay(source);

    return true;
}

void OpenALAudioRender::removeALSource()
{
    QMutexLocker locker(&mutex);

    alSourceStop(source);
    alSourcei(source, AL_BUFFER, 0);
    
    alDeleteBuffers(NUM_BUFFERS, buffers);
    alDeleteSources(1, &source);

    deviceRef --;

    if(deviceRef <= 0)
    {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(context);
        alcCloseDevice(device);

        device  = nullptr;
        context = nullptr;
    } else
    {
        device  = nullptr;
        context = nullptr;
    }
    
}

void OpenALAudioRender::onRenderFrame(const std::shared_ptr<AVFrame> &frame)
{
    frameQueue.push(frame);
}



