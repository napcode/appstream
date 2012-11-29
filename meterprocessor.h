#ifndef METERPROCESSOR_H
#define METERPROCESSOR_H

#include "config.h"
#include "processor.h"

/**
 * MeterProcessor implements a VU meter
 * details of the implementation are taken from 
 * Fons Adriaensen's brilliant jmeters
 * http://kokkinizita.linuxaudio.org/linuxaudio/downloads/index.html
 */
class MeterProcessor : public Processor
{
	public:
		MeterProcessor(uint8_t channels);
		~MeterProcessor();
		void process(sample_t *in, sample_t *out, uint32_t frames);
		
        sample_t getPeak(uint8_t channel) const;
	private:
		void initPeaks();

		sample_t *_peaks;
		float *_z1;
		float *_z2;
		float *_m;
		bool  *_res;

		float _w;
		float _g;
};

#endif
