#include "dsp.h"
#include <cassert>
#include "peakprocessor.h"

    DSP::DSP(uint8_t channels)
:   _active(false), 
    _blockSize(DSP_BLOCKSIZE), 
    _numChannels(channels)
{
    _inbuffer.init(DSP_RINGSIZE);
    _buffers[0] = 0;
    _buffers[1] = 0;
    setSize();
}
DSP::~DSP()
{
    delete[] _buffers[0];
    delete[] _buffers[1];
}
void DSP::setSize()
{
    if(_buffers[0]) {
        delete[] _buffers[0];
        _buffers[0] = 0;
    }
    if(_buffers[1]) {
        delete[] _buffers[1];
        _buffers[1] = 0;
    }
    _buffers[0] = new sample_t[_blockSize];
    _buffers[1] = new sample_t[_blockSize];
    assert(_buffers[0] && _buffers[1]);
}
void DSP::defaultSetup()
{
    _signalChain.push_back(new PeakProcessor(2));
}
void DSP::run()
{
    _active = true;
    sample_t peaks[2];
    while(1)
    {
        _work.lock();
        while(_inbuffer.getFillLevel() < _blockSize) {
            _workCondition.wait(&_work);
        }

        if (!_active)
            return;

        // data available -> take a block & process it
        uint32_t r = _inbuffer.read(_buffers[0], _blockSize);
        _work.unlock();

        // process signal chain
        SignalChain::iterator it = _signalChain.begin();
        while(it != _signalChain.end()) {
            (*it)->process(_buffers[0],_buffers[1], _blockSize);
            if ((*it)->getType() == Processor::PEAK) {
                PeakProcessor *p = static_cast<PeakProcessor*>(*it);

                emit newPeaks(p->getPeak(0),p->getPeak(1));
            }
            // now reuse/swap buffers for next processor
            //sample_t* tmp = _buffers[0];
            //_buffers[0] = _buffers[1];
            //_buffers[1] = tmp;
            it++;
        }
        if(!_active)
            return;
    }
}
void DSP::disable()
{
    _active = false;
    _workCondition.wakeAll();
}
void DSP::feed(const sample_t* buffer, unsigned long frames)
{
    // FIXME waittime should be based on the current samplerate
    if(_work.tryLock(5)) {
        _inbuffer.write(buffer, frames);
        _work.unlock();
        if (_inbuffer.getFillLevel() >= _blockSize)
            _workCondition.wakeOne();
    }
    else {
        emit stateChanged(QString("Dropping frames. DSP appears too busy."));
    }    
}


