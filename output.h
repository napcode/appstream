#ifndef OUTPUT_H
#define OUTPUT_H

#include <vector>

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QString>

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

    void disable();

    virtual bool init() = 0;

    virtual ~Output();

    /* output can be everything. samples or bytes */
    virtual void output(const char* buffer, uint32_t size) = 0;
    
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
        delete _encoder;
        _encoder = e;
    }
    void feed(const sample_t* buffer, uint32_t samples);
    virtual void run();

    const QString& getName() const { return _name; }
    void setName(const QString& name) { _name = name; }
signals:
    void message(QString msg) const;
    void warn(QString msg) const;
    void error(QString msg) const;
protected:
    QString _name;
    bool _active;
    OutputType _type;
    Encoder *_encoder;
   // uint32_t _blockSize;
	RingBuffer<sample_t> _inbuffer;

	QMutex _work;
    QWaitCondition _workCondition;
};

typedef std::vector<Output *> OutputList;

#endif
