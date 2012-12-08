#ifndef DSP_H
#define DSP_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

#include "ringbuffer.h"
#include "config.h"

// processors
#include "processor.h"
#include "meterprocessor.h"

// outputs 
#include "output.h"

// encoders
#include "encoder.h"

class DSP : public QThread
{
    Q_OBJECT
public:
    DSP(uint8_t channels);
    ~DSP();

    // add peaks and maybe more
    void defaultSetup();
    ProcessorChain& getProcessorChain() const;
    void setProcessorChain(ProcessorChain s);

    void feed(const sample_t* buffer, uint32_t samples);
    void run();
    bool isActive() const { return _active; }
    void disable();

signals:
    void message(QString s) const;
    void newPeaks(MeterValues m);
public slots:
	void addFileRecorder();

private:
    void setSize();
    void initPeaks();
    void checkPeaks();

	bool _active;
    RingBuffer<sample_t> _inbuffer;
    QMutex _work;
	QMutex _outputLock;
    QWaitCondition _workCondition;

    uint8_t _numChannels;
    sample_t *_readbuffer;
    sample_t *_writebuffer;
    ProcessorChain _processorChain;
    OutputChain _outputChain;
};

#endif // DSP_H
