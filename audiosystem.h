#ifndef AUDIOSYSTEM_H
#define AUDIOSYSTEM_H

#include <QObject>
#include <QList>
#include <QPair>

#include <portaudio.h>
#include <stdint.h>

#include "config.h"

class DSP;

typedef int32_t int24_t;

namespace AudioSystem {

enum SAMPLEFORMAT {
    INT8, 
    INT16, 
    INT24, 
    INT32, 
    FLOAT
};
struct Mode
{
    int sampleRate;
    int numChannels;
    SAMPLEFORMAT sampleFormat;
};

typedef QPair<QString,int> Device;
typedef QList<Device >  DeviceList;


class Manager : public QObject
{
    Q_OBJECT
public:
    enum State
    {
        INVALID = 0,
        INITIALIZED, 
        READY, 
        STREAMING
    };

    static Manager& getInstance();

    bool init();
    DeviceList getDeviceList() const;
    Device getDeviceByName(const QString &name);

    bool checkModeSupported(const Device &d, const Mode &m) const;

    bool openDeviceStream();
    void startDeviceStream();
    void stopDeviceStream();
    bool closeDeviceStream();

    DSP* getDSP() { return _dsp; }
    const DSP* getDSP() const { return _dsp; }
    void setDSP(DSP *dsp) { _dsp = dsp; }

    Mode& getCurrentMode() { return _streamingMode; }
    State getState() const  { return _state; }
signals:
    void message(QString text) const;
    void error(QString text) const;
    void warn(QString text) const;
private:
    Manager();
    ~Manager();

    static int _PAcallback(const void* input,
                            void *output,
                            unsigned long frameCount,
                            const PaStreamCallbackTimeInfo* ti,
                            PaStreamCallbackFlags statusFlags,
                            void *user);
    PaSampleFormat getSampleFormat(SAMPLEFORMAT t) const;
    void convert(int8_t* input, uint32_t len, sample_t *out);
    void convert(int16_t* input, uint32_t len, sample_t *out);
    void convert24(int32_t* input, uint32_t len, sample_t *out);
    void convert(int32_t* input, uint32_t len, sample_t *out);
    void convert(float* input, uint32_t len, sample_t *out);

    State _state;
    static Manager* _instance;
    PaStream *_stream;

    Mode _streamingMode;
    DSP *_dsp;
};
}
#endif // AUDIOSYSTEM_H
