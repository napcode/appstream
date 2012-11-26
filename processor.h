#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <vector>
#include <stdint.h>
#include "config.h"

class Processor
{
	public:
		Processor(uint8_t channels);
		virtual ~Processor() {};
		virtual void process(sample_t *in, sample_t *out, uint32_t frames) = 0;

		uint8_t getChannels() const { return _numChannels; }		
	protected:
		uint8_t _numChannels;		
};

typedef std::vector<Processor*> SignalChain;

#endif