#include "encoder.h"
#include "logger.h"
Encoder::Encoder(EncoderConfig c)
: 	_config(c), 
	_initialized(false),
	_buffer(0),
    _bufferSize(0),
    _bufferValid(0)
{
	connect(this, SIGNAL(message(QString)), Logger::getInstance(), SLOT(message(QString)));
	connect(this, SIGNAL(warn(QString)), Logger::getInstance(), SLOT(warn(QString)));
	connect(this, SIGNAL(error(QString)), Logger::getInstance(), SLOT(error(QString)));
}
Encoder::~Encoder()
{
	disconnect(this, SIGNAL(message(QString)), Logger::getInstance(), SLOT(message(QString)));
	disconnect(this, SIGNAL(warn(QString)), Logger::getInstance(), SLOT(warn(QString)));
	disconnect(this, SIGNAL(error(QString)), Logger::getInstance(), SLOT(error(QString)));
}
