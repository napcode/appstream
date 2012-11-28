#include "peakprocessor.h"
#include <cmath>
#include <cstring>
#include <iostream>

PeakProcessor::PeakProcessor(uint8_t channels)
: Processor(channels)
{
	_peaks = new sample_t[_numChannels];
	setType(Processor::PEAK);
}
PeakProcessor::~PeakProcessor()
{
	delete[] _peaks;
}
void PeakProcessor::process(sample_t *in, sample_t *out, uint32_t frames)
{
	initPeaks();
    for(uint32_t s = 0; s < frames; s += _numChannels) {
        for(uint8_t c = 0; c < _numChannels; ++c) {
            if (fabs(in[s+c]) > _peaks[c])
                _peaks[c] = fabs(in[s+c]);
			// copy in to out
			out[s+c] = in[s+c];
        }
	}
}
void PeakProcessor::initPeaks()
{
    memset(_peaks, 0, sizeof(sample_t)*_numChannels);
}
sample_t PeakProcessor::getPeak(uint8_t channel) const
{
	if (channel >= _numChannels)
    	return 0;
    return _peaks[channel];
}
