#ifndef DSP_H
#define DSP_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include "ringbuffer.h"
#include "config.h"
#include "processor.h"

class DSP : public QThread
{
    Q_OBJECT
public:
    DSP(uint8_t channels);
    ~DSP();

    // add peaks and maybe more
    void defaultSetup();
    SignalChain& getSignalChain() const;
    void setSignalChain(SignalChain s);

    void feed(const sample_t* buffer, unsigned long frames);
    void run();
    bool isActive() const { return _active; }
    void disable();

    RingBuffer<sample_t>& getBuffer() { return _inbuffer; }
    const RingBuffer<sample_t>& getBuffer() const { return _inbuffer; }
signals:
    void stateChanged(QString s);
    void newPeaks(sample_t l,sample_t r);

private:
    void setSize();
    void initPeaks();
    void checkPeaks();

	bool _active;
    RingBuffer<sample_t> _inbuffer;
    QMutex _work;
    QWaitCondition _workCondition;
    unsigned int _blockSize;
    uint8_t _numChannels;
    sample_t *_buffers[2];
    SignalChain _signalChain;
};

#endif // DSP_H