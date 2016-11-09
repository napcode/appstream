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

class DSP : public QThread {
    Q_OBJECT
public:
    DSP();
    ~DSP();

    // add peaks and maybe more
    ProcessorChain& getProcessorChain() { return _processorChain; }
    const ProcessorChain& getProcessorChain() const { return _processorChain; }
    void addProcessor(Processor* p);
    void removeProcessor(Processor* p);

    OutputList& getOutputList() { return _outputList; }
    const OutputList& getOutputList() const { return _outputList; }
    void addOutput(Output* o);
    void removeOutput(Output* o);

    void setNumChannels(uint8_t channels) { _numChannels = channels; }
    uint8_t getNumChannels() const { return _numChannels; }

    void feed(const sample_t* buffer, uint32_t samples);
    void run();
    bool isActive() const { return _active; }
    void disable();
    void reset();
signals:
    void message(QString s) const;
    void warn(QString s) const;
    void error(QString s) const;
    void newPeaks(MeterValues m);

private:
    void allocBuffers();
    void initPeaks();
    void checkPeaks();

    bool _active;
    RingBuffer<sample_t> _inbuffer;
    QMutex _work;
    QMutex _outputLock;
    QMutex _processorLock;
    QWaitCondition _workCondition;

    uint8_t _numChannels;
    sample_t* _readbuffer;
    sample_t* _writebuffer;
    ProcessorChain _processorChain;
    OutputList _outputList;
};

#endif // DSP_H
