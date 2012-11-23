#include "audiosystem.h"


using namespace AudioSystem;

Manager* Manager::_instance = 0;

Manager& Manager::getInstance()
{
    if(!_instance) {
        _instance = new Manager;
    }
    return *_instance;
}

Manager::Manager()
 : _stream(0),
   _isDeviceStreaming(false)
{

}
Manager::~Manager()
{
    Pa_Terminate();

}

bool Manager::init()
{
    PaError e;
    e = Pa_Initialize();
    if(e != paNoError) {
        emit stateChanged(QString("PortAudio init failed"));
        emit stateChanged(QString(Pa_GetErrorText(e)));        
    }
    emit stateChanged(QString("Audio initialized"));
    emit stateChanged(QString(Pa_GetVersionText()));

    return true;
}
DeviceList Manager::getDeviceList() const
{
    DeviceList l;
    int devcount = Pa_GetDeviceCount();
    if (devcount < 0) {
        QString msg("Error getting device list");
        msg.append(Pa_GetErrorText(devcount));
        emit stateChanged(msg);
    }
    for(int i = 0; i < devcount; ++i) {
        const PaDeviceInfo *pdi = Pa_GetDeviceInfo(i);
        if (pdi == 0) {
            QString msg("Error getting device info: " + i);
            emit stateChanged(msg);
            continue;
        }
        if (pdi->maxInputChannels <= 0)
            continue;
        Device d;
        d.first = QString(pdi->name);
        d.second = i;
        l.append(d);
    }
    return l;
}
bool Manager::checkModeSupported(const Device &d, const Mode &m) const
{
    const PaDeviceInfo *pdi = Pa_GetDeviceInfo(d.second);
    PaStreamParameters params;
    params.device = d.second;
    params.channelCount = m.numChannels;
    switch(m.bitsPerSample) {
        case 8:
            params.sampleFormat = paInt8; break;
        case 16:
            params.sampleFormat = paInt16; break;
        case 24:
            params.sampleFormat = paInt24; break;
        case 32:
            params.sampleFormat = paInt32; break;
        default:
            params.sampleFormat = paInt16;
    }
    params.suggestedLatency = pdi->defaultHighInputLatency;
    params.hostApiSpecificStreamInfo = NULL;
    if(Pa_IsFormatSupported(&params ,0 , m.sampleRate) == paNoError)
        return true;
    return false;
}
bool Manager::openDeviceStream()
{
    PaError err;
    PaStreamParameters params;
    params.device = 2;
    params.channelCount = 1;
    params.sampleFormat = paInt16;
    params.suggestedLatency = 2048;
    params.hostApiSpecificStreamInfo = 0;

    err = Pa_OpenStream(&_stream, 
                        &params, 
                        0,
                        44100,
                        44100,
                        paNoFlag,
                        Manager::_PAcallback,
                        this);
    if(err != paNoError) {
        emit stateChanged("Error opening device");
        emit stateChanged(QString(Pa_GetErrorText(err)));
        return false;
    }
    Pa_StartStream(_stream);
    _isDeviceStreaming = true;
    return true;
}
bool Manager::closeDeviceStream()
{
    if (!_isDeviceStreaming)
        return false;

    Pa_StopStream(_stream);
    Pa_CloseStream(_stream);
    _isDeviceStreaming = false;
    return true;
}
int Manager::_PAcallback(  const void* input,
                            void *output,
                            unsigned long frameCount,
                            const PaStreamCallbackTimeInfo* ti,
                            PaStreamCallbackFlags statusFlags,
                            void *user)
{
    Manager *self = static_cast<Manager*>(user);
    emit self->stateChanged(QString::number(frameCount));
    return 0;
}
