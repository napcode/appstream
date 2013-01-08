#include "dsp.h"
#include "logger.h"

// encoders
#include "encoder.h"

// outputs 
#include "output.h"

#include <cassert>

#include <QSettings>
#include <QMutexLocker>

DSP::DSP()
	:   _active(false),	
	_numChannels(0)
{
    connect(this, SIGNAL(message(QString)), Logger::getInstance(), SLOT(message(QString)));
    connect(this, SIGNAL(warn(QString)), Logger::getInstance(), SLOT(warn(QString)));
    connect(this, SIGNAL(error(QString)), Logger::getInstance(), SLOT(error(QString)));

	_inbuffer.init(DSP_RINGSIZE);
	_readbuffer = 0;
	_writebuffer = 0;
	allocBuffers();
}
DSP::~DSP()
{
    disconnect(this, SIGNAL(message(QString)), Logger::getInstance(), SLOT(message(QString)));
    disconnect(this, SIGNAL(warn(QString)), Logger::getInstance(), SLOT(warn(QString)));
    disconnect(this, SIGNAL(error(QString)), Logger::getInstance(), SLOT(error(QString)));

	disable();
	{
		ProcessorChain::iterator it = _processorChain.begin();
		while (it != _processorChain.end())
		{
			delete *it;
			it++;
		}
		_processorChain.clear();
	}
	{
		OutputList::iterator it = _outputList.begin();
		while (it != _outputList.end())
		{
			delete *it;
			it++;
		}
		_outputList.clear();
	}
	delete[] _readbuffer;
	delete[] _writebuffer;
}
void DSP::reset()
{
	disable();
	{
		ProcessorChain::iterator it = _processorChain.begin();
		while (it != _processorChain.end())
		{
			delete *it;
			it++;
		}
		_processorChain.clear();
	}
	{
		OutputList::iterator it = _outputList.begin();
		while (it != _outputList.end())
		{
			delete *it;
			it++;
		}
		_outputList.clear();
	}	
}
void DSP::allocBuffers()
{
	if (_readbuffer)
	{
		delete[] _readbuffer;
		_readbuffer = 0;
	}
	if (_writebuffer)
	{
		delete[] _writebuffer;
		_writebuffer = 0;
	}
	_readbuffer = new sample_t[DSP_RINGSIZE];
	_writebuffer = new sample_t[DSP_RINGSIZE];
	assert(_readbuffer && _writebuffer);
}
void DSP::addProcessor(Processor *p)
{
    QMutexLocker ml(&_processorLock);
    _processorChain.push_back(p);
}
void DSP::removeProcessor(Processor *p)
{
    QMutexLocker ml(&_processorLock);
    _processorChain.remove(p);
    delete p;
}
void DSP::addOutput(Output *o)
{
    QMutexLocker ml(&_outputLock);
    _outputList.push_back(o);
}
void DSP::removeOutput(Output *o)
{
    QMutexLocker ml(&_outputLock);
    _outputList.remove(o);
    delete o;
}
void DSP::run()
{
	_active = true;
	while (_active)
	{
		_work.lock();
		while (_inbuffer.getFillLevel() < DSP_BLOCKSIZE)
		{
			_workCondition.wait(&_work);
			if (!_active) {
				_work.unlock();
				return;
			}
		}

		// data available -> take a block & process it
		uint32_t read = _inbuffer.read(_readbuffer, DSP_RINGSIZE);
		assert(read >= DSP_BLOCKSIZE);
		_work.unlock();
		// process signal chain
		{
			QMutexLocker ml(&_processorLock);
			ProcessorChain::iterator it = _processorChain.begin();
			while (it != _processorChain.end())
			{
				(*it)->process(_readbuffer, _writebuffer, read);
				if ((*it)->getType() == Processor::METER)
				{
					MeterProcessor *p = static_cast<MeterProcessor *>(*it);
					emit newPeaks(p->getValues());
				} else {
					// now reuse/swap buffers for next processor
					sample_t *tmp = _readbuffer;
					_readbuffer = _writebuffer;
					_writebuffer = tmp;
				}
				it++;
			}
		}
		// send buffer to output
		{
			// outputs can be modified on runtime
			QMutexLocker ml(&_outputLock);
			OutputList::iterator it = _outputList.begin();
			while (it != _outputList.end())
			{
				(*it)->feed(_readbuffer, read);
				it++;
			}
		}
		//FileLogger::instance().log(_readbuffer, read, _numChannels);
	}
}
void DSP::disable()
{
	if (_active)
	{
		_active = false;
		_workCondition.wakeAll();
		wait();
		OutputList::iterator it = _outputList.begin();
		while (it != _outputList.end())
		{
			(*it)->disable();
			it++;
		}
	}
}
void DSP::feed(const sample_t *buffer, uint32_t samples)
{
	if(!_active)
		return;
	// FIXME waittime should be based on the current samplerate
	if (_work.tryLock(5))
	{
		_inbuffer.write(buffer, samples);
				
		if (_inbuffer.getFillLevel() >= DSP_BLOCKSIZE) {
			_work.unlock();
			_workCondition.wakeOne();
		}
		else 
			_work.unlock();
	}
	else
	{
		emit message(QString("Dropping frames. DSP appears too busy."));
	}
}
