#include "output.h"

Output::Output()
    : _active(false),
      _type(INVALID),
      _encoder(0),
      _blockSize(512)
{
    _inbuffer.init(OUTPUT_RINGSIZE);
}
Output::~Output()
{
    if(_active)
        disable();
    delete _encoder;
}
void Output::disable()
{
    if (_active)
    {
        _active = false;
        _workCondition.wakeAll();
        wait();
    }
}
void Output::run()
{
    _active = true;
    uint32_t read;
    sample_t buffer[512];
    while (_active)
    {
        _work.lock();
        while (_inbuffer.getFillLevel() == 0)
        {
            _workCondition.wait(&_work);
            if (!_active)
                return;
        }
        read = _inbuffer.read(buffer, 512);
        _work.unlock();

        if (_encoder) {
            _encoder->encode(buffer,512);
            output(_encoder->getBuffer(),_encoder->getBufferValid());
        }
        //sleep(1);
        //std::cout << "output::run" << std::endl;
    }
}
void Output::feed(const sample_t *buffer, uint32_t frames)
{
    // FIXME waittime should be based on the current samplerate
    if (_work.tryLock(5))
    {
        _inbuffer.write(buffer, frames);
        _work.unlock();
        if (_inbuffer.getFillLevel() >= 0)
            _workCondition.wakeOne();
    }
    else
    {
        // do something about it?
    }
}
