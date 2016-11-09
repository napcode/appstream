#include "audiosystem.h"
#include "dsp.h"
#include "logger.h"
#include <QSettings>
#include <math.h>
using namespace AudioSystem;

std::unique_ptr<Manager> Manager::create()
{
    return std::unique_ptr<Manager>(new Manager);
}

Manager::Manager()
: _state(INVALID),
  _stream(0),
  _dsp(0)
{
    connect(this, SIGNAL(message(QString)), Logger::getInstance(), SLOT(message(QString)));
    connect(this, SIGNAL(warn(QString)), Logger::getInstance(), SLOT(warn(QString)));
    connect(this, SIGNAL(error(QString)), Logger::getInstance(), SLOT(error(QString)));
}
Manager::~Manager()
{
    if (_state == STREAMING)
        stopDeviceStream();
    if (_state == READY)
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
    if (e != paNoError) {
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
    if (_state < INITIALIZED) {
        emit warn("not init");
        return l;
    }
    int devcount = Pa_GetDeviceCount();

    if (devcount < 0) {
        QString msg("getting device list");
        msg.append(Pa_GetErrorText(devcount));
        emit error(msg);
    }
    for (int i = 0; i < devcount; ++i) {
        const PaDeviceInfo* pdi = Pa_GetDeviceInfo(i);
        if (pdi == 0) {
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
Device Manager::getDeviceByName(const QString& name)
{
    DeviceList l = getDeviceList();
    AudioSystem::DeviceList::Iterator it = l.begin();
    while (it != l.end()) {
        if (it->first == name)
            return *it;
        ++it;
    }
    Device d("INVALID", -1);
    return d;
}
bool Manager::checkModeSupported(const Device& d, const Mode& m) const
{
    if (_state < INITIALIZED)
        return false;

    const PaDeviceInfo* pdi = Pa_GetDeviceInfo(d.second);
    PaStreamParameters params;
    params.device = d.second;
    params.channelCount = m.numChannels;
    params.suggestedLatency = pdi->defaultHighInputLatency;
    params.sampleFormat = getSampleFormat(m.sampleFormat);
    params.hostApiSpecificStreamInfo = NULL;
    if (Pa_IsFormatSupported(&params, 0, m.sampleRate) == paNoError)
        return true;
    return false;
}
PaSampleFormat Manager::getSampleFormat(SAMPLEFORMAT t) const
{
    switch (t) {
        case INT8:
            return paInt8;
        case INT16:
            return paInt16;
        case INT24:
            return paInt24;
        case INT32:
            return paInt32;
        case FLOAT:
            return paFloat32;
        default:
            return paFloat32;
    }
}
bool Manager::openDeviceStream()
{
    if (_state != INITIALIZED)
        return false;
    PaError err;
    PaStreamParameters params;

    QSettings s;
    s.beginGroup("audio");
    if (!s.contains("deviceName") || !s.contains("sampleRate") || !s.contains("sampleFormat") || !s.contains("numChannels")) {
        emit error("invalid device config found. Please reconfigure.");
        return false;
    }
    Device d = getDeviceByName(s.value("deviceName").toString());
    if (d.second == -1) {
        emit error("configured device not found");
        return false;
    }

    Mode m;
    m.sampleRate = s.value("sampleRate").toInt();
    m.numChannels = s.value("numChannels").toInt();
    m.sampleFormat = static_cast<SAMPLEFORMAT>(s.value("sampleFormat").toInt());
    params.sampleFormat = getSampleFormat(m.sampleFormat);

    // FIXME we'll set that for now to
    //params.sampleFormat = paInt16;
    //m.sampleFormat = 16;
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
    if (err != paNoError) {
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
    if (_state != READY)
        return;
    emit message("Starting device stream...");
    _state = STREAMING;
    Pa_StartStream(_stream);
}
void Manager::stopDeviceStream()
{
    if (_state != STREAMING)
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
int Manager::_PAcallback(const void* input,
    void* output,
    unsigned long frames,
    const PaStreamCallbackTimeInfo* ti,
    PaStreamCallbackFlags statusFlags,
    void* user)
{
    // get "this" pointer
    Manager* self = static_cast<Manager*>(user);
    const sample_t* in = static_cast<const sample_t*>(input);
    static sample_t* scratch = 0;
    static uint32_t length = 0;
    if (self->_dsp) {
        uint8_t channels = self->_streamingMode.numChannels;
        if (length < (frames * channels)) {
            int32_t size;
            switch (self->_streamingMode.sampleFormat) {
                case INT8:
                    size = 8;
                    break;
                case INT16:
                    size = 16;
                    break;
                case INT24:
                    size = 32;
                    break;
                case INT32:
                    size = 32;
                    break;
                case FLOAT:
                    size = 32;
                    break;
                default:
                    assert(0);
            }
            scratch = ( sample_t* )realloc(scratch, frames * channels * size);
            length = frames * channels;
        }
        switch (self->_streamingMode.sampleFormat) {
            case INT8:
                self->convert(( int8_t* )input, frames * channels, scratch);
                break;
            case INT16:
                self->convert(( int16_t* )input, frames * channels, scratch);
                break;
            case INT24:
                self->convert24(( int32_t* )input, frames * channels, scratch);
                break;
            case INT32:
                self->convert(( int32_t* )input, frames * channels, scratch);
                break;
            case FLOAT:
                self->convert(( float* )input, frames * channels, scratch);
                break;
            default:
                return paAbort;
        }
        short* v = ( short* )input;
        self->_dsp->feed(scratch, frames * channels);
    }
    return paContinue;
}
void Manager::convert(int8_t* in, uint32_t len, sample_t* out)
{
    for (uint32_t i = 0; i < len; ++i)
        out[i] = in[i] / 255.0f;
}
void Manager::convert(int16_t* in, uint32_t len, sample_t* out)
{
    for (uint32_t i = 0; i < len; ++i)
        out[i] = in[i] / 32768.0f;
}
void Manager::convert24(int32_t* in, uint32_t len, sample_t* out)
{
}
void Manager::convert(int32_t* in, uint32_t len, sample_t* out)
{
    for (uint32_t i = 0; i < len; ++i)
        out[i] = in[i] / ( double )INT_MAX;
}
void Manager::convert(float* in, uint32_t len, sample_t* out)
{
    for (uint32_t i = 0; i < len; ++i)
        out[i] = in[i];
}
