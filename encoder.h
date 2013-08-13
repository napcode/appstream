#ifndef ENCODER_H
#define ENCODER_H

#include <QString>
#include <QObject>

#include "config.h"
#include "ringbuffer.h"

struct EncoderConfig
{
    enum Mode
    {
        CBR,            /* 64 - 320 */
        VBR             /* 0.0 - 1.0 */
    };
    EncoderConfig()
    :   sampleRateIn(44100),
        sampleRateOut(44100),
        numInChannels(2),
        mode(CBR), 
        quality(128)
    {}
    uint32_t sampleRateIn;
    uint32_t sampleRateOut;
    uint8_t numInChannels;
    Mode mode;
    float quality;
};

class Encoder : public QObject
{
    Q_OBJECT
public:
    Encoder(EncoderConfig c = EncoderConfig());
    virtual ~Encoder() ;

    virtual bool init() = 0;
    bool isInitialized() const { return _initialized; }

    /**
     * @brief setup encoding process. this is called once at the beginning of the encoding process
     */
    virtual void setup() = 0;

    /**
     * @brief encode
     * @param buffer ptr to sample buffer. buffer is assumed to be interleaved
     * @param samples number of samples in buffer
     * @return true if no error occurred
     */
    virtual void encode(sample_t *buffer, uint32_t samples) = 0;
    
    /**
     * @brief finialize is called once after the all input has been encoded. this can be used to finalize the output
     */
     virtual void finalize() = 0;

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
    void message(QString msg) const;
    void warn(QString msg) const;
    void error(QString msg) const;

protected:
    EncoderConfig _config;    
	QString _name;
	bool _initialized;
    char* _buffer;
    uint32_t _bufferSize;
    uint32_t _bufferValid;
    //RingBuffer<sample_t> _inbuffer;
};

#endif // ENCODER_H

