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
        numInChannels(1),
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
    bool encode(short *buffer, uint32_t samples);
    bool encode(float *buffer, uint32_t samples);

	// reimplemented from Encoder
    QString getVersion() const;
    QString getFileExtension() const { return QString("mp3"); }
private:
	bool handleRC(int rc);
	void resize(uint32_t newSize);

	static void lameError(const char* format, va_list ap);
	static void lameDebug(const char* format, va_list ap);
	static void lameMessage(const char* format, va_list ap);

	ConfigLame _config;
	lame_global_flags *_lgf;
	uint32_t _allocedFrames;
};

#endif // ENCODERLAME_H
