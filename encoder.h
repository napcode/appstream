#ifndef ENCODER_H
#define ENCODER_H

#include <QString>
#include <QObject>

#include "config.h"
#include "ringbuffer.h"

class Encoder : public QObject
{
    Q_OBJECT
public:
    Encoder();
    virtual ~Encoder() ;

    virtual bool init() = 0;
    bool isInitialized() const { return _initialized; }
    /**
     * @brief encode
     * @param buffer ptr to sample buffer. should be in the range [-32768,32768]. buffer is assumed to be interleaved
     * @param samples number of samples in buffer
     * @return true if no error occurred
     */
    virtual bool encode(short *buffer, uint32_t samples) = 0;
    
    /**
     * @brief encode
     * @param buffer ptr to sample buffer. should be in the range [-1,1]. buffer is assumed to be interleaved
     * @param samples number of samples un buffer
     * @return true if no error occurred     
     */
    virtual bool encode(float *buffer, uint32_t samples) = 0;

    const QString& getName() const { return _name; }
    void setName(const QString& name) { _name = name; }

    const char* getBuffer() const { return _buffer; }
    uint32_t getBufferSize() const { return _bufferSize; }
    uint32_t getBufferValid() const { return _bufferValid; }
    /**
     * get a string with library name & version
     */
    virtual QString getVersion() const = 0;

    virtual QString getFileExtension() const = 0;
signals:
    void message(QString s) const;
protected:
	QString _name;
	bool _initialized;
    char* _buffer;
    uint32_t _bufferSize;
    uint32_t _bufferValid;
    //RingBuffer<sample_t> _inbuffer;
};

#endif // ENCODER_H

