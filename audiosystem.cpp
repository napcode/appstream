#include "audiosystem.h"
#include "dsp.h"
#include "logger.h"
#include <QSettings>
#include <math.h>
using namespace AudioSystem;

Manager *Manager::_instance = 0;

Manager &Manager::getInstance()
{
    if (!_instance)
    {
        _instance = new Manager;
    }
    return *_instance;
}

Manager::Manager()
    :   _state(INVALID), 
        _stream(0),
		_dsp(0)
{
    connect(this, SIGNAL(message(QString)), Logger::getInstance(), SLOT(message(QString)));
    connect(this, SIGNAL(warn(QString)), Logger::getInstance(), SLOT(warn(QString)));
    connect(this, SIGNAL(error(QString)), Logger::getInstance(), SLOT(error(QString)));
}
Manager::~Manager()
{
    if(_state == STREAMING) 
        stopDeviceStream();
    if(_state == READY)
        closeDeviceStream();
    Pa_Terminate();  
    disconnect(this, SIGNAL(message(QString)), Logger::getInstance(), SLOT(message(QString)));
    disconnect(this, SIGNAL(warn(QString)), Logger::getInstance(), SLOT(warn(QString)));
    disconnect(this, SIGNAL(error(QString)), Logger::getInstance(), SLOT(error(QString)));  
}

bool Manager::init()
{
    PaError e;
    e = Pa_Initialize();
    if (e != paNoError)
    {
        emit message(QString("PortAudio init failed"));
        emit message(QString(Pa_GetErrorText(e)));
        return false;
    }
    emit message(QString("Audio initialized"));
    emit message(QString(Pa_GetVersionText()));
    _state = INITIALIZED;
    return true;
}
DeviceList Manager::getDeviceList() const
{
    DeviceList l;
    if(_state != INITIALIZED)
        return l;
    int devcount = Pa_GetDeviceCount();
    if (devcount < 0)
    {
        QString msg("getting device list");
        msg.append(Pa_GetErrorText(devcount));
        emit error(msg);
    }
    for (int i = 0; i < devcount; ++i)
    {
        const PaDeviceInfo *pdi = Pa_GetDeviceInfo(i);
        if (pdi == 0)
        {
            QString msg("Error getting device info: " + i);
            emit error(msg);
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
Device Manager::getDeviceByName(const QString &name)
{
    DeviceList l = getDeviceList();
    AudioSystem::DeviceList::Iterator it = l.begin();
    while (it != l.end())
    {
        if (it->first == name)
            return *it;
        ++it;
    }
    Device d("INVALID", -1);
    return d;
}
bool Manager::checkModeSupported(const Device &d, const Mode &m) const
{
    if(_state != INITIALIZED)
        return false;

    const PaDeviceInfo *pdi = Pa_GetDeviceInfo(d.second);
    PaStreamParameters params;
    params.device = d.second;
    params.channelCount = m.numChannels;
    switch (m.bitsPerSample)
    {
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
    if (Pa_IsFormatSupported(&params , 0 , m.sampleRate) == paNoError)
        return true;
    return false;
}
bool Manager::openDeviceStream()
{
    if (_state != INITIALIZED)
        return false;
    PaError err;
    PaStreamParameters params;

    QSettings s;
    s.beginGroup("audio");
    if (!s.contains("deviceName") || !s.contains("sampleRate")
            || !s.contains("bitsPerSample") || !s.contains("numChannels"))
    {
        emit error("invalid device config found. Please reconfigure.");
        return false;
    }
    Device d = getDeviceByName(s.value("deviceName").toString());
    if (d.second == -1)
    {
        emit error("configured device not found");
        return false;
    }

    Mode m;
    m.sampleRate = s.value("sampleRate").toInt();
    m.numChannels = s.value("numChannels").toInt();    
    m.bitsPerSample = s.value("bitsPerSample").toInt();
    switch (m.bitsPerSample)
    {
    case 8:
        params.sampleFormat = paInt8; break;
    case 16:
        params.sampleFormat = paInt16; break;
    case 24:
        params.sampleFormat = paInt24; break;
    case 32:
        params.sampleFormat = paInt32; break;
    default:
        params.sampleFormat = paFloat32;
    }

    // FIXME we'll set that for now to
    params.sampleFormat = paInt16;
    m.bitsPerSample = 16;
    //m.numChannels = 1;
    // END FIXME

    params.device = d.second;
    params.channelCount = m.numChannels;
    params.suggestedLatency = Pa_GetDeviceInfo(params.device)->defaultLowInputLatency;
    params.hostApiSpecificStreamInfo = 0;
    emit message("Opening device...");
    err = Pa_OpenStream(&_stream,
                        &params,
                        0,
                        m.sampleRate,
                        PA_FRAMES,
                        paNoFlag,
                        Manager::_PAcallback,
                        this);
    if (err != paNoError)
    {
        emit error("Unable to open device");
        _state = INVALID;
        emit message(QString(Pa_GetErrorText(err)));
        return false;
    }

    _streamingMode = m;
    _state = READY;
    return true;
}
void Manager::startDeviceStream()
{
    if(_state != READY)
        return;
    emit message("Starting device stream...");
    _state = STREAMING;
    Pa_StartStream(_stream);  
}
void Manager::stopDeviceStream()
{
    if(_state != STREAMING)
        return;
    emit message("Stopping device stream...");
    Pa_StopStream(_stream);
    _state = READY;
}
bool Manager::closeDeviceStream()
{
    if (_state != READY)
        return false;
    emit message("Closing device...");
    Pa_CloseStream(_stream);
    _state = INITIALIZED;
    return true;
}
int Manager::_PAcallback(const void *input,
                         void *output,
                         unsigned long frames,
                         const PaStreamCallbackTimeInfo *ti,
                         PaStreamCallbackFlags statusFlags,
                         void *user)
{
    // get "this" pointer
    Manager *self = static_cast<Manager *>(user);
    static uint32_t k = 0;
    const sample_t *in = static_cast<const sample_t *>(input);
    if(self->_dsp) {        
        short *v = (short*)input;
        uint8_t channels = self->_streamingMode.numChannels;
        /*
        for(uint32_t i = 0; i < frames * channels; i += channels ) {
            for(uint8_t c = 0; c < channels; ++c) {
                v[i+c] = 30000 * sin((2*3.1415*((55.0*(c+1))/44100.0)*k));
            }
            ++k;
        }

        */       
		self->_dsp->feed(in, frames*channels);
    }

    return paContinue;
}
