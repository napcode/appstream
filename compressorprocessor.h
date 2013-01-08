#ifndef COMPRESSORPROCESSOR_H
#define COMPRESSORPROCESSOR_H 1

#include "config.h"
#include "processor.h"

class CompressorProcessor : public Processor
{
    Q_OBJECT
public:
    CompressorProcessor(uint8_t channels, uint32_t samplerate);
    ~CompressorProcessor();

    void process(sample_t *in, sample_t *out, uint32_t samples);

public slots:
	void setThreshold(double th) 		{ _threshold = th; }
	void setSlope(double slope) 		{ _slope = slope; }
	void setTimeLookAhead(double tla)	{ _tla = tla; }
	void setTimeWindow(double tw)		{ _tw = tw; }
	void setTimeAttack(double ta)		{ _ta = ta; }
	void setTimeRelease(double tr) 		{ _tr = tr; }
    void setGain(int g)                 { _g = g; }
private:
    /* data */
    double _threshold;
    double _slope;
    double _tla;
    double _tw;
    double _ta;
    double _tr;
    int _g;

};
#endif
