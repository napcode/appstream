#ifndef AUDIOSYSTEM_H
#define AUDIOSYSTEM_H

#include <QObject>
#include <QList>
#include <QPair>

#include <portaudio.h>
#include <stdint.h>

class DSP;

namespace AudioSystem {

struct Mode
{
    int sampleRate;
    int numChannels;
    int bitsPerSample;
};

typedef QPair<QString,int> Device;
typedef QList<Device >  DeviceList;


class Manager : public QObject
{
    Q_OBJECT
public:
    enum State
    {
        INVALID,
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

    State getState() const { return _state; }
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
    State _state;
    static Manager* _instance;
    PaStream *_stream;

    Mode _streamingMode;
    DSP *_dsp;
};
}
#endif // AUDIOSYSTEM_H
