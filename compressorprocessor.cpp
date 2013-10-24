#include "compressorprocessor.h"
#include <math.h>
#include <stdio.h>

CompressorProcessor::CompressorProcessor(uint8_t channels, uint32_t samplerate)
    :	Processor(channels, samplerate),
        _threshold(20.0f),
        _slope(0.0f),
        _tla(2),
        _tw(100),
        _ta(10),
        _tr(3000),
        _g(10.0f)
{

}
CompressorProcessor::~CompressorProcessor()
{

}
void CompressorProcessor::process(sample_t *in, sample_t *out, uint32_t samples)
{
    sample_t *buf[2];
    buf[0] = new sample_t[samples];
    buf[1] = new sample_t[samples];
    uint32_t j, k;
    j = 0;
    for(uint32_t i = 0; i < samples; i+=2) {
        buf[0][j] = in[i];
        ++j;
    }
    if (_numChannels == 1)
        processMono(in, out, samples);
    else if (_numChannels == 2)
        processStereo(buf[0], buf[1], j);
    k = j;
    j = 0;
    for(uint32_t i = 0; i < k; ++i) {
        out[j] = buf[1][i];
        j += 2;
    }
    delete[] buf[0];
    delete[] buf[1];
}
void CompressorProcessor::processMono(sample_t *in, sample_t *out, uint32_t samples)
{
    /*
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

    double mean = 0.0;
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
    for (int i = 0; i < samples; ++i) {
        // now compute RMS
        double  summ = 0;

        // for each sample in window
        for (int j = 0; j < nrms; ++j) {
            int     lki = i + j + lhsmp;
            double  smp;

            // if we in bounds of signal?
            // if so, convert to mono
            if (lki < samples)
                smp = in[lki];
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
        if (env > threshold) {
            gain = gain - (env - threshold) * slope;
            mean += gain;
        }

        // result - hard kneed compressed channels...
        out[i] = in[i] * gain * _g;
    }
    printf("%lf\n", mean/samples);
}
void CompressorProcessor::processStereo(sample_t *in, sample_t *out, uint32_t samples)
{
    /*
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
    double mean = 0.0;
    double threshold = _threshold * 0.01;          // threshold to unity (0...1)
    double slope = _slope * 0.01;              // slope to unity
    double tla = _tla * 1e-3;                // lookahead time to seconds
    double tw = _tw * 1e-3;               // window time to seconds
    double ta = _ta * 1e-3;               // attack time to seconds
    double tr = _tr * 1e-3;               // release time to seconds

    // attack and release "per sample decay"
    double  att = (ta == 0.0) ? (0.0) : exp (-1.0 / (_sampleRate * ta));
    double  rel = (tr == 0.0) ? (0.0) : exp (-1.0 / (_sampleRate * tr));

    // envelope
    double  env = 0.0;

    // sample offset to lookahead wnd start
    int     lhsmp = (int) (_sampleRate * tla);

    // samples count in lookahead window
    int     nrms = (int) (_sampleRate * tw);

    // for each sample...
    for (int i = 0; i < samples; ++i) {
        // now compute RMS
        double  summ = 0;

        // for each sample in window
        for (int j = 0; j < nrms; ++j) {
            int     lki = i + j + lhsmp;
            double  smp;

            // if we in bounds of signal?
            // if so, convert to mono
            if (lki < samples)
                smp = in[lki];
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
        if (env > threshold) {
            gain = gain - (env - threshold) * slope;
            mean += gain;
        }

        // result - hard kneed compressed channels...
        out[i] = in[i] * gain * _g;
    }
}