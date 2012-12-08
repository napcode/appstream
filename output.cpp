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
    sample_t buffer[OUTPUT_RINGSIZE];
    while (_active)
    {
        _work.lock();
        while (_inbuffer.getFillLevel() == 0)
        {
            _workCondition.wait(&_work);
            if (!_active) {
                _work.unlock();
                return;
            }
        }
        // FIXME ringbuffer read amount unclear
        read = _inbuffer.read(buffer, OUTPUT_RINGSIZE);
        //assert(read!=0);
        //filelog(buffer, read, 2);

        _work.unlock();

        if (_encoder) {
            _encoder->encode(buffer,read);
            output(_encoder->getBuffer(),_encoder->getBufferValid());
        }
        //sleep(1);
        //std::cout << "output::run" << std::endl;
    }
}
void Output::feed(const sample_t *buffer, uint32_t frames)
{
    if(!_active)
        return;
    // FIXME waittime should be based on the current samplerate
    if (_work.tryLock(5))
    {

        _inbuffer.write(buffer, frames);
        _work.unlock();
        if (_inbuffer.getFillLevel() != 0)
            _workCondition.wakeOne();
    }
    else
    {
        // do something about it?
        emit message("Dropout in output occurred");
    }
}
