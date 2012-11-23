#ifndef AUDIOSYSTEM_H
#define AUDIOSYSTEM_H

#include <QObject>
#include <QList>
#include <QPair>

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
signals:
    void stateChanged(QString text) const;

private:
    Manager();
    ~Manager();

    static Manager* _instance;
};
}
#endif // AUDIOSYSTEM_H
