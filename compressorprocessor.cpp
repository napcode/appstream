#include "compressorprocessor.h"

CompressorProcessor::CompressorProcessor(uint8_t channels, uint32_t samplerate)
	:	Processor(channels, samplerate), 
        _threshold(50.0f), 
        _slope(50.0f), 
        _tla(2), 
        _tw(100), 
        _ta(5), 
        _tr(1000)
{

}
CompressorProcessor::~CompressorProcessor()
{

}
void CompressorProcessor::process(sample_t *in, sample_t *out, uint32_t samples)
{/*
	void compress
    (
        float*  wav_in,     // signal
        int     n,          // N samples
        double  threshold,  // threshold (percents)
        double  slope,      // slope angle (percents)
        int     _sampleRate,         // sample rate (smp/sec)
        double  _tla,        // lookahead  (ms)
        double  _tw,       // window time (ms)
        double  _ta,       // attack time  (ms)
        double  _tr        // release time (ms)
    )*/
    typedef float   stereodata[2];
    stereodata*     wav = (stereodata*) in; // our stereo signal
    stereodata* output = (stereodata*) out;
    double threshold = _threshold * 0.01;          // threshold to unity (0...1)
    double slope = _slope * 0.01;              // slope to unity
    _tla *= 1e-3;                // lookahead time to seconds
    _tw *= 1e-3;               // window time to seconds
    _ta *= 1e-3;               // attack time to seconds
    _tr *= 1e-3;               // release time to seconds
    
    // attack and release "per sample decay"
    double  att = (_ta == 0.0) ? (0.0) : exp (-1.0 / (_sampleRate * _ta));
    double  rel = (_tr == 0.0) ? (0.0) : exp (-1.0 / (_sampleRate * _tr));

    // envelope
    double  env = 0.0;

    // sample offset to lookahead wnd start
    int     lhsmp = (int) (_sampleRate * _tla);

    // samples count in lookahead window
    int     nrms = (int) (_sampleRate * _tw);

    // for each sample...
    for (int i = 0; i < (samples / _numChannels); ++i)
    {
        // now compute RMS
        double  summ = 0;

        // for each sample in window
        for (int j = 0; j < nrms; ++j)
        {
            int     lki = i + j + lhsmp;
            double  smp;

            // if we in bounds of signal?
            // if so, convert to mono
            if (lki < samples)
                smp = 0.5 * wav[lki][0] + 0.5 * wav[lki][1];
            else
                smp = 0.0;      // if we out of bounds we just get zero in smp

            summ += smp * smp;  // square em..
        }

        double  rms = sqrt (summ / nrms);   // root-mean-square

        // dynamic selection: attack or release?
        double  theta = rms > env ? att : rel;

        // smoothing with capacitor, envelope extraction...
        // here be aware of pIV denormal numbers glitch
        env = (1.0 - theta) * rms + theta * env;

        // the very easy hard knee 1:N compressor
        double  gain = 1.0;
        if (env > threshold)
            gain = gain - (env - threshold) * slope;

        // result - two hard kneed compressed channels...
        output[i][0] = wav[i][0] * gain * _g;
        output[i][1] = wav[i][1] * gain * _g;
    }
}
