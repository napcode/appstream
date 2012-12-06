#include "encodervorbis.h"

EncoderVorbis::EncoderVorbis(void)
{
}


EncoderVorbis::~EncoderVorbis(void)
{
}

bool EncoderVorbis::init()
{
	return true;
}

bool EncoderVorbis::encode(short *buffer, uint32_t samples)
{
	return true;
}
bool EncoderVorbis::encode(float *buffer, uint32_t samples)
{
	return true;
}
QString EncoderVorbis::getVersion() const
{
	return QString("0.0");
}
QString EncoderVorbis::getFileExtension() const
{
	return QString("ogg");
}