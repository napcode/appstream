#ifndef PEAKPROCESSOR_H
#define PEAKPROCESSOR_H

#include "config.h"
#include "processor.h"

class PeakProcessor : public Processor
{
	public:
		PeakProcessor(uint8_t channels);
		~PeakProcessor();
		void process(sample_t *in, sample_t *out, uint32_t frames);
		
        sample_t getPeak(uint8_t channel) const;
	private:
		sample_t *_peaks;

		void initPeaks();
};

#endif
