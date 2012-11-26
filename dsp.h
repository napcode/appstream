#ifndef DSP_H
#define DSP_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include "ringbuffer.h"

class DSP : public QThread
{
public:
    DSP();
    ~DSP();

    void feed(const short* buffer, unsigned long frames);
    void run();
    bool isActive() const { return _active; }
    void disable() { _active = false; }

    RingBuffer<short>& getBuffer() { return _ringbuffer; }
    const RingBuffer<short>& getBuffer() const { return _ringbuffer; }

    void getPeaks(short *l, short *r);
private:
    void setSize();
    void checkPeak();

	bool _active;
    RingBuffer<short> _ringbuffer;
    QMutex _bufferlock;
    QMutex _work;
    QWaitCondition _workCondition;
    unsigned int _blockSize;
    short *_workBuffer;
    short _max_l, _max_r;
};

#endif // DSP_H
