#include "encoderlame.h"

EncoderLame::EncoderLame(ConfigLame c)
    : _config(c)
{

}
EncoderLame::~EncoderLame()
{
    if (_lgf)
    {
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

    if ((rc = lame_init_params(_lgf)) < 0)
    {
        emit error("unable to init lame");
        emit error("Channels " + QString::number(_config.numInChannels));
        emit error("RateIn " + QString::number(_config.sampleRateIn));
        emit error("RateOut " + QString::number(_config.sampleRateOut));
        emit error("BitRate " + QString::number(_config.bitRate));
        return false;
    }
    else
    {
        _initialized = true;
        emit message("Lame initialized");
        emit message("Version: " + getVersion());
    }
    // default output buffer size. see lame.h
    resize(DSP_BLOCKSIZE);
    return true;
}
void EncoderLame::setup()
{
    // nothing to do here
}
void EncoderLame::encode(short *buffer, uint32_t samples)
{
    int rc;

    if (samples == 0 || !isInitialized())
        return;

    if (_allocedFrames < samples)
        resize(samples);

    if (_config.numInChannels == 2)
    {
        rc = lame_encode_buffer_interleaved(_lgf, buffer, samples / _config.numInChannels, reinterpret_cast<unsigned char *>(_buffer), _bufferSize);
    }
    else if (_config.numInChannels == 1)
    {
        rc = lame_encode_buffer(_lgf, buffer, buffer, samples, reinterpret_cast<unsigned char *>(_buffer), _bufferSize);
    }
    else
    {
        emit error("Lame can't handle more than 2 channels. Should never happen.");
        assert(0);
    }

    if (rc >= 0)
        _bufferValid = rc;
    else
    {
        _bufferValid = 0;
        handleRC(rc);
    }
}
void EncoderLame::encode(float *buffer, uint32_t samples)
{
    int rc;

    if (samples == 0 || !isInitialized())
        return;

    if (_allocedFrames < samples)
        resize(samples);

    if (_config.numInChannels == 2)
        rc = lame_encode_buffer_interleaved_ieee_float(_lgf, buffer, samples / _config.numInChannels, reinterpret_cast<unsigned char *>(_buffer), _bufferSize);
    else if (_config.numInChannels == 1)
        rc = lame_encode_buffer_ieee_float(_lgf, buffer, buffer, samples, reinterpret_cast<unsigned char *>(_buffer), _bufferSize);
    else
    {
        emit error("Lame can't handle more than 2 channels.");
        assert(0);
    }

    if (rc >= 0)
        _bufferValid = rc;
    else
    {
        _bufferValid = 0;
        handleRC(rc);
    }
}
void EncoderLame::finalize()
{
    
}

void EncoderLame::handleRC(int rc) const
{
    switch (rc)
    {
    case -1:
        emit error("Lame: out buffer to small"); break;
    case -2:
        emit error("Lame: unable to allocate memory"); break;
    case -3:
        emit error("Lame: init not called. Should never happen."); break;
    case -4:
        emit error("Lame: psycho acoustic problem occurred"); break;
    default:
        emit error("Lame: unknown error occurred. ");
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
    if (!isInitialized())
    {
        emit error("lame is not initialized. Can't get version info.");
        return info;
    }
    QString lame_version(get_lame_version());

    return lame_version;
}
