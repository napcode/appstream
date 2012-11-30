#ifndef METERPROCESSOR_H
#define METERPROCESSOR_H

#include "config.h"
#include "processor.h"
#include <vector>
#include <bitset>
/**
 * MeterProcessor implements a VU meter
 * details of the implementation are taken from 
 * Fons Adriaensen's brilliant jmeters
 * http://kokkinizita.linuxaudio.org/linuxaudio/downloads/index.html
 */
typedef std::vector<float> Vecf;
typedef std::vector<float> Vecb;
typedef std::vector<float> MeterValues;

class MeterProcessor : public Processor
{
	public:
		MeterProcessor(uint8_t channels);
		~MeterProcessor();
		void process(sample_t *in, sample_t *out, uint32_t frames);
		
        MeterValues getValues();
	private:
		void initPeaks();

        MeterValues _v;
		Vecf _z1;
		Vecf _z2;
		Vecf _m;
		Vecb _reset;

		float _w;
		float _g;
};

#endif
