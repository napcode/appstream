#include "encoderlame.h"
#include "logger.h"
EncoderLame::EncoderLame(ConfigLame c)
: _config(c)
{
    connect(this, SIGNAL(message(QString)), Logger::getInstance(), SLOT(log(QString)));
}
EncoderLame::~EncoderLame()
{
	if(_lgf) {
		lame_close(_lgf);
    	_lgf = 0;
	}
	delete[] _buffer;
}
bool EncoderLame::init()
{
	if (isInitialized())
		return true;

	int rc;
	_lgf = lame_init();
	lame_set_errorf(_lgf, EncoderLame::lameError);
	lame_set_debugf(_lgf, EncoderLame::lameDebug);
	lame_set_msgf(_lgf, EncoderLame::lameMessage);
	/*
    emit message(QString::number(_config.numInChannels));
    emit message(QString::number(_config.sampleRateIn));
    emit message(QString::number(_config.sampleRateOut));
    emit message(QString::number(_config.bitRate));
	*/
    lame_set_num_channels(_lgf, _config.numInChannels);
    lame_set_in_samplerate(_lgf, _config.sampleRateIn);
    lame_set_out_samplerate(_lgf, _config.sampleRateOut);
    lame_set_brate(_lgf, _config.bitRate);

    if((rc = lame_init_params(_lgf)) < 0){
        emit message("Error: unable to init lame");
        return false;
    }
    else {
        _initialized = true;
        emit message("Lame initialized");
        emit message("Version: " + getVersion());
    }
    // default output buffer size. see lame.h
	resize(DSP_BLOCKSIZE);
    return true;
}
bool EncoderLame::encode(short *buffer, uint32_t samples)
{
	int rc;

    if(samples == 0 || !isInitialized())
        return false;

    if(_allocedFrames < samples)
        resize(samples);
    
    if(_config.numInChannels == 2) {
        rc = lame_encode_buffer_interleaved(_lgf, buffer, (samples>>1), reinterpret_cast<unsigned char*>(_buffer), _bufferSize);
    }
    else {
        rc = lame_encode_buffer(_lgf, buffer, buffer, samples, reinterpret_cast<unsigned char*>(_buffer), _bufferSize);
    }
    if(rc >= 0)
        _bufferValid = rc;
    else
        _bufferValid = 0;

    return handleRC(rc);	
}
bool EncoderLame::encode(float *buffer, uint32_t samples)
{
	int rc;

    if(samples == 0 || !isInitialized())
        return false;

    if(_allocedFrames < samples)
        resize(samples);
    emit message("float encode");
    if(_config.numInChannels == 2)
        rc = lame_encode_buffer_interleaved_ieee_float(_lgf, buffer, (samples>>1), reinterpret_cast<unsigned char*>(_buffer), _bufferSize);
    else 
        rc = lame_encode_buffer_ieee_float(_lgf, buffer, buffer, samples, reinterpret_cast<unsigned char*>(_buffer), _bufferSize);
    if(rc >= 0)
        _bufferValid = rc;
    else
        _bufferValid = 0;

    return handleRC(rc);	
}
bool EncoderLame::handleRC(int rc)
{
    switch(rc) {
    	case -1:
    		emit message("Lame error: out buffer to small"); return false;
    	case -2:
    		emit message("Lame error: unable to allocate memory"); return false;
    	case -3:
    		emit message("Lame error: init not called. Should never happen."); return false;
    	case -4:
    		emit message("Lame error: psycho acoustic problem occurred"); return false;
    	default:
    		return true;
	}	
}
void EncoderLame::resize(uint32_t newSize)
{
	delete[] _buffer;
	_allocedFrames = newSize;
	// default output buffer size. see lame.h
	_bufferSize = 1.25 * _allocedFrames + 7200;
	_buffer = new char[_bufferSize];
}
QString EncoderLame::getVersion() const
{
	QString info;
	if(!isInitialized()) {
        emit message("Error: lame is not initialized. Can't get info log.");        
		return info;
	}
	QString lame_version(get_lame_version());

    return lame_version;
}
void EncoderLame::lameError(const char *format, va_list ap)
{
	vfprintf(stdout, format, ap);
}
void EncoderLame::lameDebug(const char *format, va_list ap)
{
	vfprintf(stdout, format, ap);
}
void EncoderLame::lameMessage(const char *format, va_list ap)
{
	vfprintf(stdout, format, ap);
}
