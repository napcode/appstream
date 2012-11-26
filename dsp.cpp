#include "dsp.h"
#include <cassert>

    DSP::DSP()
:_active(false), 
    _blockSize(2048), 
    _workBuffer(0)
{
    _ringbuffer.init(16384);
    setSize();
}
DSP::~DSP()
{
    delete[] _workBuffer;
}
void DSP::setSize()
{
    if(_workBuffer) {
        delete[] _workBuffer;
        _workBuffer = 0;
    }
    _workBuffer = new short[_blockSize];
    assert(_workBuffer);
}
void DSP::run()
{
    _active = true;
    while(_active)
    {
        _work.lock();
        _workCondition.wait(&_work);
        _work.unlock();
        checkPeak();
    }
}
void DSP::feed(const short* buffer, unsigned long frames)
{
    // maybe trylock would be better here
    // we could drop frames if DSP is too busy
    _bufferlock.lock();
    _ringbuffer.write(buffer, frames);
    _bufferlock.unlock();
    _workCondition.wakeOne();
    // notify others
}
void DSP::getPeaks(short *l, short *r)
{
    *l = _max_l;
    *r = _max_r;
}
void DSP::checkPeak()
{
    _max_l = 0;
    _max_r = 0;
    _bufferlock.lock();
    if(!_ringbuffer.isEmpty() && _ringbuffer.getFillLevel() > _blockSize) {
        _ringbuffer.read(_workBuffer, _blockSize);
        _bufferlock.unlock();
        for(unsigned int i = 0; i < _blockSize; i += 2) {
            if (_workBuffer[i] > _max_l) {
                _max_l = _workBuffer[i];
            }
            if (_workBuffer[i+1] > _max_r) {
                _max_r = _workBuffer[i+1];
            }
        }
    }
}
