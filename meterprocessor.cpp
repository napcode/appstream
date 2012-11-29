#include "meterprocessor.h"
#include <cmath>
#include <cstring>
#include <iostream>

MeterProcessor::MeterProcessor(uint8_t channels)
    : Processor(channels)
{
    _peaks = new sample_t[_numChannels];
    _z1 = new sample_t[_numChannels];
    _z2 = new sample_t[_numChannels];
    _res = new bool[_numChannels];
    setType(Processor::METER);
    _w = 11.1f / 44100;
    _g = 1.5f * 1.571f;
}
MeterProcessor::~MeterProcessor()
{
    delete[] _peaks;
}
void MeterProcessor::process(sample_t *in, sample_t *out, uint32_t frames)
{
   // initPeaks();
    for (uint8_t c = 0; c < _numChannels; ++c)
    {
        float z1, z2, m, t1, t2;
        z1 = _z1[c];
        z2 = _z2[c];
        m = _res[c] ? 0 : _peaks[c];
        _res[c] = false;
        //n /= 4;
        for (uint32_t s = c; s < frames; s += _numChannels)
        {
        	sample_t *p = in + s;
            t2 = z2 / 2;
            t1 = fabsf (*(p) - t2);
            z1 += _w * (t1 - z1);
            t1 = fabsf (*(p + _numChannels) - t2);
            z1 += _w * (t1 - z1);
            t1 = fabsf (*(p + _numChannels*2) - t2);
            z1 += _w * (t1 - z1);
            t1 = fabsf (*(p + _numChannels*3) - t2);
            z1 += _w * (t1 - z1);
            z2 += 4 * _w * (z1 - z2);
            if (z2 > m)
                m = z2;
            out[s + c] = in[s + c];
        }
        _z1[c] = z1;
        _z2[c] = z2 + 1e-10f;
        _peaks[c] = m;
    }
}
void MeterProcessor::initPeaks()
{
    memset(_peaks, 0, sizeof(sample_t)*_numChannels);
    memset(_z1, 0, sizeof(sample_t)*_numChannels);
    memset(_z2, 0, sizeof(sample_t)*_numChannels);
    memset(_res, 0, sizeof(bool)*_numChannels);
}
sample_t MeterProcessor::getPeak(uint8_t channel) const
{
    _res[channel] = true;
    if (channel >= _numChannels)
        return 0;
    return _peaks[channel];
}
