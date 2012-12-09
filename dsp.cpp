#include "dsp.h"
#include "logger.h"
#include "filelogger.h"
// encoders
#include "encoderlame.h"
#include "encodervorbis.h"
// outputs 
#include "outputfile.h"
#include "outputicecast.h"

#include <cassert>

#include <QSettings>
#include <QMutexLocker>

DSP::DSP(uint8_t channels)
	:   _active(false),	
	_numChannels(channels)
{
    connect(this, SIGNAL(message(QString)), Logger::getInstance(), SLOT(log(QString)));
    connect(this, SIGNAL(warn(QString)), Logger::getInstance(), SLOT(warn(QString)));
    connect(this, SIGNAL(error(QString)), Logger::getInstance(), SLOT(error(QString)));

	_inbuffer.init(DSP_RINGSIZE);
	_readbuffer = 0;
	_writebuffer = 0;
	setSize();
}
DSP::~DSP()
{
    disconnect(this, SIGNAL(message(QString)), Logger::getInstance(), SLOT(log(QString)));
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
		OutputChain::iterator it = _outputChain.begin();
		while (it != _outputChain.end())
		{
			delete *it;
			it++;
		}
		_outputChain.clear();
	}
	delete[] _readbuffer;
	delete[] _writebuffer;
}
void DSP::setSize()
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
void DSP::defaultSetup()
{
	_processorChain.push_back(new MeterProcessor(_numChannels));
	QSettings s;
	s.beginGroup("record");
    if(1)
    {
    	OutputIceCast *oic = new OutputIceCast;
    	//EncoderLame *e = new EncoderLame;
    	//e->init();
    	//oic->setEncoder(e);
    	oic->init();
    	emit message(QString("added stream ") + oic->getVersion());

    }
	if(s.value("enabled").toBool()) {
		addFileRecorder();
	}
}
void DSP::addFileRecorder()
{
	QSettings s;
	s.beginGroup("record");
	OutputFile *f = 0;
	Encoder *e = 0;
	if(!s.contains("recordPath") || !s.contains("recordFileName") ||
		!s.contains("encoder") || !s.contains("encoderBitRate") ||
		!s.contains("encoderSampleRate"))
	{
		emit message("Error: missing recorder config");
		return;
	}
	if(s.value("encoder").toString() == QString("Lame MP3")) {
		ConfigLame c;
		c.bitRate = s.contains("encoderBitRate") ? s.value("encoderBitRate").toInt() : 64;
		c.sampleRateOut = s.contains("encoderSampleRate") ? s.value("encoderSampleRate").toInt() : 44100;
		c.numInChannels = _numChannels;
		e = new EncoderLame(c);
	}
	else if(s.value("encoder").toString() == QString("Ogg Vorbis"))
		e = new EncoderVorbis;
	assert(e);
	if(!e->init())
		return;
	f = new OutputFile(s.value("recordPath").toString(),
		s.value("recordFileName").toString());
	f->setEncoder(e);
	if(!f->init())
		return;
	emit message("adding file recorder");
	_outputLock.lock();
	_outputChain.push_back(f);
	_outputLock.unlock();
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
					//sample_t *tmp = _readbuffer;
					//_readbuffer = _writebuffer;
					//_writebuffer = tmp;
				}
				it++;
			}
		}
		// send buffer to output
		{
			// outputs can be modified on runtime
			_outputLock.lock();
			OutputChain::iterator it = _outputChain.begin();
			while (it != _outputChain.end())
			{
				(*it)->feed(_readbuffer, read);
				it++;
			}
			_outputLock.unlock();
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
		OutputChain::iterator it = _outputChain.begin();
		while (it != _outputChain.end())
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
