#ifndef ENCODERVORBIS_H
#define ENCODERVORBIS_H

#include "encoder.h"

#include <vorbis/vorbisenc.h>

struct ConfigVorbis
{
	ConfigVorbis()
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

class EncoderVorbis :
	public Encoder
{
public:
	EncoderVorbis(ConfigVorbis c = ConfigVorbis());
	~EncoderVorbis(void);
    bool init();
    bool encode(short *buffer, uint32_t samples);
    bool encode(float *buffer, uint32_t samples);
    QString getVersion() const;
    QString getFileExtension() const;
private:
	void handleRC(int rc) const;
	void writeHeader();
	void resize(uint32_t newsize);
private:
	ConfigVorbis _c;
	ogg_stream_state _oss;
	ogg_page _opg;
	ogg_packet _opk;
	vorbis_info _vi;
	vorbis_comment _vc;
	vorbis_block _vb;
	vorbis_dsp_state _vdsp;
};

#endif