#include "meterprocessor.h"
#include <cmath>
#include <cstring>
#include <iostream>

MeterProcessor::MeterProcessor(uint8_t channels)
    : Processor(channels)
{
    _v.resize(channels);
    _z1.resize(_numChannels);
    _z2.resize(_numChannels);
    _reset.resize(_numChannels);
    setType(Processor::METER);
    _w = 11.1f / 44100;
    _g = 1.5f * 1.571f;
}
MeterProcessor::~MeterProcessor()
{

}
void MeterProcessor::process(sample_t *in, sample_t *out, uint32_t samples)
{
   // initPeaks();
    for (uint8_t c = 0; c < _numChannels; ++c)
    {
        float z1, z2, m, t1, t2;
        z1 = _z1[c];
        z2 = _z2[c];
        m = _reset[c] ? 0 : _v[c];
        _reset[c] = false;
        //n /= 4;
        for (uint32_t s = c; s < (samples-(_numChannels*3)); s += _numChannels)
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
        _v[c] = m;
    }
}
void MeterProcessor::initPeaks()
{
    for(uint8_t i = 0; i < _numChannels; ++i) {
        _v[i] = 0.0f;
        _z1[i] = 0.0f;
        _z2[i] = 0.0f;
        _reset[i] = false;
    }
}
MeterValues MeterProcessor::getValues()
{
    for(uint8_t i = 0; i < _numChannels; ++i) {
        _reset[i] = true;
        _v[i] = clamp(fabs(_v[i] / 32768), 0.0f, 1.0f);
    }
    return _v;
}
