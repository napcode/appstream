#ifndef AUDIOSYSTEM_H
#define AUDIOSYSTEM_H

#include <QObject>
#include <QList>
#include <QPair>

#include <portaudio.h>

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

    static Manager& getInstance();

    bool init();
    DeviceList getDeviceList() const;

    bool checkModeSupported(const Device &d, const Mode &m) const;
    bool openDeviceStream();
    bool closeDeviceStream();
    Device getDeviceByName(const QString &name);
signals:
    void stateChanged(QString text) const;

private:
    Manager();
    ~Manager();


    static int _PAcallback(const void* input,
                            void *output,
                            unsigned long frameCount,
                            const PaStreamCallbackTimeInfo* ti,
                            PaStreamCallbackFlags statusFlags,
                            void *user);

    static Manager* _instance;
    PaStream *_stream;

    bool _isDeviceStreaming;
    Mode _streamingMode;
};
}
#endif // AUDIOSYSTEM_H
