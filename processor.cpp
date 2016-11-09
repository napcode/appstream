#include "processor.h"

Processor::Processor(uint8_t channels, uint32_t samplerate)
: _numChannels(channels),
  _sampleRate(samplerate),
  _type(INVALID)
{
}