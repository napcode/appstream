#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <list>
#include <stdint.h>
#include "config.h"


class Processor
{
public:
    enum ProcessorType
    {
        INVALID,
        METER,
        EQ,
        LIMIT,
        COMPRESSOR
    };

    Processor(uint8_t channels);
    virtual ~Processor() {};
    virtual void process(sample_t *in, sample_t *out, uint32_t samples) = 0;

    uint8_t getChannels() const
    {
        return _numChannels;
    }

    ProcessorType getType() const
    {
        return _type;
    }
    void setType(ProcessorType t)
    {
        _type = t;
    }
protected:
    uint8_t _numChannels;
    ProcessorType _type;
};

typedef std::list<Processor *> ProcessorChain;

#endif
