#ifndef ENCODERVORBIS_H
#define ENCODERVORBIS_H

#include "encoder.h"
class EncoderVorbis :
	public Encoder
{
public:
	EncoderVorbis(void);
	~EncoderVorbis(void);
    bool init();
    bool encode(short *buffer, uint32_t samples);
    bool encode(float *buffer, uint32_t samples);
    QString getVersion() const;
    QString getFileExtension() const;
};

#endif