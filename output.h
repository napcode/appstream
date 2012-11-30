#ifndef OUTPUT_H
#define OUTPUT_H

#include <vector>

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

#include "config.h"
#include "encoder.h"
#include "ringbuffer.h"

/**
 * @brief abstract output type
 */
class Output : public QThread
{
	Q_OBJECT
public:
	enum OutputType
    {
    	INVALID,
    	DEVICE,
    	STREAM,
    	FILE
    };
    Output();

    virtual void init() = 0;

    virtual ~Output();

    OutputType getType() const
    {
        return _type;
    }
    void setType(OutputType t)
    {
        _type = t;
    }
    Encoder* getEncoder() const
    {
        return _encoder;
    }

    void setEncoder(Encoder *e)
    {
        if(_encoder)
            delete _encoder;
        _encoder = e;
    }
	void feed(const sample_t* buffer, unsigned long frames);
    virtual void run();
protected:
    bool _active;
    OutputType _type;
    Encoder *_encoder;
	RingBuffer<sample_t> _inbuffer;
	QMutex _work;
    QWaitCondition _workCondition;
};

typedef std::vector<Output *> OutputChain;

#endif
