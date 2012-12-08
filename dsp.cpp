#include "dsp.h"
#include "logger.h"
// encoders
#include "encoderlame.h"
#include "encodervorbis.h"
// outputs 
#include "outputfile.h"

#include <cassert>

#include <QSettings>

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
	delete[] _buffers[0];
	delete[] _buffers[1];
}
void DSP::setSize()
{
	if (_buffers[0])
	{
		delete[] _buffers[0];
		_buffers[0] = 0;
	}
	if (_buffers[1])
	{
		delete[] _buffers[1];
		_buffers[1] = 0;
	}
	_buffers[0] = new sample_t[_blockSize];
	_buffers[1] = new sample_t[_blockSize];
	assert(_buffers[0] && _buffers[1]);
}
void DSP::defaultSetup()
{
	_processorChain.push_back(new MeterProcessor(_numChannels));
	QSettings s;
	s.beginGroup("record");
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
	while (1)
	{
		_work.lock();
		while (_inbuffer.getFillLevel() < _blockSize)
		{
			_workCondition.wait(&_work);
			if (!_active)
				return;
		}

		// data available -> take a block & process it
		_inbuffer.read(_buffers[0], _blockSize);
		_work.unlock();

		// process signal chain
		{
			ProcessorChain::iterator it = _processorChain.begin();
			while (it != _processorChain.end())
			{
				(*it)->process(_buffers[0], _buffers[1], _blockSize);
				if ((*it)->getType() == Processor::METER)
				{
					MeterProcessor *p = static_cast<MeterProcessor *>(*it);
					emit newPeaks(p->getValues());
				} else {
					// now reuse/swap buffers for next processor
					//sample_t *tmp = _buffers[0];
					//_buffers[0] = _buffers[1];
					//_buffers[1] = tmp;
				}
				it++;
			}
		}
		// send buffer to output
		{
			_outputLock.lock();
			OutputChain::iterator it = _outputChain.begin();
			while (it != _outputChain.end())
			{
				(*it)->feed(_buffers[0], _blockSize);
				it++;
			}
			_outputLock.unlock();
		}
		//sleep(1);
		//std::cout << "dsp::run" << std::endl;
		if (!_active)
			return;
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
void DSP::feed(const sample_t *buffer, uint32_t frames)
{
	// FIXME waittime should be based on the current samplerate
	if (_work.tryLock(5))
	{
		_inbuffer.write(buffer, frames);
		_work.unlock();
		if (_inbuffer.getFillLevel() >= _blockSize)
			_workCondition.wakeOne();
	}
	else
	{
		emit message(QString("Dropping frames. DSP appears too busy."));
	}
}


