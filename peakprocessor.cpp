#include "peakprocessor.h"
#include <cmath>
#include <cstring>

PeakProcessor::PeakProcessor(uint8_t channels)
: Processor(channels)
{
	_peaks = new sample_t[_numChannels];
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
const sample_t* PeakProcessor::getPeaks() const
{
    return _peaks;
}
