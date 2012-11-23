#include "audiosystem.h"
#include <portaudio.h>

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
        QString msg("PortAudio init failed: ");
        msg.append(Pa_GetErrorText(e));
        emit stateChanged(msg);
    }
    emit stateChanged(QString("Audio initialized: " + QString(Pa_GetVersionText())));

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
