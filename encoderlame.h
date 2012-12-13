#ifndef ENCODERLAME_H
#define ENCODERLAME_H

#include "encoder.h"

#include <stdint.h>

#include <lame/lame.h>

struct ConfigLame
{
	ConfigLame() 
    :	sampleRateIn(44100),
        sampleRateOut(44100),
        numInChannels(2),
        bitRate(128)
	{}
	uint32_t sampleRateIn;
	uint32_t sampleRateOut;
	uint8_t numInChannels;
	uint16_t bitRate;
};

class EncoderLame : public Encoder
{
public:
    EncoderLame(ConfigLame c = ConfigLame());
    ~EncoderLame();

    // reimplemented from Encoder    
    bool init();
    // reimplemented from Encoder
    void setup();
    void encode(short *buffer, uint32_t samples);
    void encode(float *buffer, uint32_t samples);
    void finalize();

	// reimplemented from Encoder
    QString getVersion() const;
    QString getFileExtension() const { return QString("mp3"); }
private:
	void handleRC(int rc) const;
	void resize(uint32_t newSize);

private:
	ConfigLame _config;
	lame_global_flags *_lgf;
	uint32_t _allocedFrames;
};

#endif // ENCODERLAME_H
