#include "audiosystem.h"
#include "dsp.h"
#include <QSettings>

#define FRAMESIZE 2048

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
Device Manager::getDeviceByName(const QString &name)
{
    DeviceList l = getDeviceList();
    AudioSystem::DeviceList::Iterator it = l.begin();
    while (it != l.end())
    {
	if (it->first == name) {
	    return *it;
	}
	++it;
    }
    Device d("INVALID",-1);
    return d;
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

    QSettings s;
    s.beginGroup("audio");
    if (!s.contains("deviceName") || !s.contains("sampleRate")
	    || !s.contains("bitsPerSample") || !s.contains("numChannels"))
    {
	emit stateChanged("Error: invalid device config found. Please reconfigure.");
	return false;
    }
    Device d = getDeviceByName(s.value("deviceName").toString());
    if (d.second == -1) {
	emit stateChanged("Error: configured device not found");
	return false;
    }

    Mode m;
    m.sampleRate = s.value("sampleRate").toInt();
    // FIXME don't use currentText here
    // store channelCount in combobox
    if (s.value("numChannels").toString() == "Mono")
	m.numChannels = 1;
    else
	m.numChannels = 2;

    m.bitsPerSample = s.value("bitsPerSample").toInt();
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

    // FIXME we'll set that for now to
    params.sampleFormat = paInt16;
    m.bitsPerSample = 16;

    params.device = d.second;
    params.channelCount = m.numChannels;
    params.suggestedLatency = FRAMESIZE;
    params.hostApiSpecificStreamInfo = 0;

    err = Pa_OpenStream(&_stream, 
	    &params, 
	    0,
	    m.sampleRate,
	    FRAMESIZE,
	    paNoFlag,
	    Manager::_PAcallback,
	    this);
    if(err != paNoError) {
    	emit stateChanged("Error opening device");
    	emit stateChanged(QString(Pa_GetErrorText(err)));
    	return false;
    }
    /* setup went well -> start device stream */
    _isDeviceStreaming = true;
    _streamingMode = m;
    emit stateChanged("Stream started...");

    Pa_StartStream(_stream);

    return true;
}
bool Manager::closeDeviceStream()
{
    if (!_isDeviceStreaming)
	   return false;

    Pa_StopStream(_stream);
    Pa_CloseStream(_stream);
    emit stateChanged("Stream stopped...");
    _isDeviceStreaming = false;
    return true;
}
int Manager::_PAcallback(const void* input,
	void *output,
	unsigned long frameCount,
	const PaStreamCallbackTimeInfo* ti,
	PaStreamCallbackFlags statusFlags,
	void *user)
{
    // get "this" pointer
    Manager *self = static_cast<Manager*>(user);
    
    // FIXME casting is rather ugly here
    self->_dsp->feed(static_cast<const short*>(input), frameCount);
    emit self->newAudioFrames();
    //self->closeDeviceStream();
    return paContinue;
}
