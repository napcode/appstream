#include "encodervorbis.h"

EncoderVorbis::EncoderVorbis(EncoderConfig c)
: Encoder(c)
{
    // 5k seems to be a good value for std settings
    resize(5000);
}


EncoderVorbis::~EncoderVorbis(void)
{
    if (!_initialized) {
        ogg_stream_clear(&_oss);
        vorbis_block_clear(&_vb);
        vorbis_dsp_clear(&_vdsp);
        vorbis_comment_init(&_vc);
        vorbis_info_clear(&_vi);
    }
    delete[] _buffer;
}

bool EncoderVorbis::init()
{
    int ret = 0;

    vorbis_info_init(&_vi);
    if (_config.mode == EncoderConfig::CBR) {
        ret = vorbis_encode_init(&_vi,
            _config.numInChannels,
            _config.sampleRateIn,
            _config.quality * 1000,
            _config.quality * 1000,
            _config.quality * 1000);
    }
    else if (_config.mode == EncoderConfig::VBR) {
        ret = vorbis_encode_init_vbr(&_vi,
            _config.numInChannels,
            _config.sampleRateIn,
            _config.quality);
    }
    if (ret != 0) {
        handleRC(ret);
        return false;
    }
    vorbis_analysis_init(&_vdsp, &_vi);
    vorbis_comment_init(&_vc);
    vorbis_comment_add_tag(&_vc, "ENCODER", "appStream");
    vorbis_block_init(&_vdsp, &_vb);

    emit message("Vorbis initialized");
    emit message("Version: " + getVersion());
    _initialized = true;
    return true;
}
void EncoderVorbis::setup()
{
    ogg_packet header;
    ogg_packet header_comm;
    ogg_packet header_code;
    ogg_stream_init(&_oss, qrand());
    vorbis_analysis_headerout(&_vdsp,
        &_vc,
        &header,
        &header_comm,
        &header_code);
    ogg_stream_packetin(&_oss, &header);
    ogg_stream_packetin(&_oss, &header_comm);
    ogg_stream_packetin(&_oss, &header_code);

    while (1) {
        int res = ogg_stream_flush(&_oss, &_opg);
        if (res == 0)
            break;
        if (_bufferSize < (_bufferValid + _opg.header_len + _opg.body_len))
            resize(_bufferValid + _opg.header_len + _opg.body_len);
        memcpy(_buffer + _bufferValid, _opg.header, _opg.header_len);
        _bufferValid += _opg.header_len;
        memcpy(_buffer + _bufferValid, _opg.body, _opg.body_len);
        _bufferValid += _opg.body_len;
    }
}
void EncoderVorbis::encode(sample_t* buffer, uint32_t samples)
{
    _bufferValid = 0;

    int eos = 0;
    int res;
    uint32_t i;
    float** vbuffer;

    vbuffer = vorbis_analysis_buffer(&_vdsp, samples);
    if (_config.numInChannels == 1) {
        for (i = 0; i < samples; ++i)
            vbuffer[0][i] = buffer[i];
    }
    else {
        for (i = 0; i < (samples >> 1); ++i) {
            vbuffer[0][i] = buffer[i << 1];
            vbuffer[1][i] = buffer[(i << 1) + 1];
        }
    }
    vorbis_analysis_wrote(&_vdsp, i);

    while (vorbis_analysis_blockout(&_vdsp, &_vb)) {
        vorbis_analysis(&_vb, &_opk);
        vorbis_bitrate_addblock(&_vb);
        while (vorbis_bitrate_flushpacket(&_vdsp, &_opk)) {
            ogg_stream_packetin(&_oss, &_opk);
            while (!eos) {
                res = ogg_stream_pageout(&_oss, &_opg);
                if (res == 0)
                    break;
                if (_bufferSize < (_bufferValid + _opg.header_len + _opg.body_len))
                    resize(_bufferValid + _opg.header_len + _opg.body_len);
                memcpy(_buffer + _bufferValid, _opg.header, _opg.header_len);
                _bufferValid += _opg.header_len;
                memcpy(_buffer + _bufferValid, _opg.body, _opg.body_len);
                _bufferValid += _opg.body_len;
                if (ogg_page_eos(&_opg))
                    eos = 1;
            }
        }
    }
}
void EncoderVorbis::finalize()
{
    int eos = 0;
    int res;

    vorbis_analysis_wrote(&_vdsp, 0);
    while (vorbis_analysis_blockout(&_vdsp, &_vb)) {
        vorbis_analysis(&_vb, &_opk);
        vorbis_bitrate_addblock(&_vb);
        while (vorbis_bitrate_flushpacket(&_vdsp, &_opk)) {
            ogg_stream_packetin(&_oss, &_opk);
            while (!eos) {
                res = ogg_stream_pageout(&_oss, &_opg);
                if (res == 0)
                    break;
                if (_bufferSize < (_bufferValid + _opg.header_len + _opg.body_len))
                    resize(_bufferValid + _opg.header_len + _opg.body_len);
                memcpy(_buffer + _bufferValid, _opg.header, _opg.header_len);
                _bufferValid += _opg.header_len;
                memcpy(_buffer + _bufferValid, _opg.body, _opg.body_len);
                _bufferValid += _opg.body_len;
                if (ogg_page_eos(&_opg))
                    eos = 1;
            }
        }
    }
}
QString EncoderVorbis::getVersion() const
{
    const char* p = vorbis_version_string();
    return QString(p);
}
QString EncoderVorbis::getFileExtension() const
{
    return QString("ogg");
}
void EncoderVorbis::handleRC(int rc) const
{
    switch (rc) {
        case OV_EFAULT:
            emit error("internal vorbis fault");
            break;
        case OV_EINVAL:
            emit error("vorbis setup seems invalid");
            break;
        case OV_EIMPL:
            emit error("vorbis mode not implemented");
            break;
        default:
            emit error("vorbis error occurred");
    }
}
void EncoderVorbis::resize(uint32_t newSize)
{
    if (newSize < _bufferSize)
        return;
    char* buffer = new char[newSize];
    assert(buffer);
    memcpy(buffer, _buffer, _bufferSize);
    delete[] _buffer;
    _bufferSize = newSize;
    _buffer = buffer;
}
