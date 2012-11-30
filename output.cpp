#include "output.h"

Output::Output()
    : _active(false),
      _type(INVALID),
      _encoder(0)
{

}
Output::~Output()
{
   // delete _encoder;
}
void Output::run()
{
    _active = true;
    while (_active)
    {
        _work.lock();
        while (_inbuffer.getFillLevel() > 0)
        {
            _workCondition.wait(&_work);
            if (!_active)
                return;
        }
    }
}
void Output::feed(const sample_t *buffer, unsigned long frames)
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
