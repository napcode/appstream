#ifndef ENCODERVORBIS_H
#define ENCODERVORBIS_H

#include "encoder.h"

#include <vorbis/vorbisenc.h>

class EncoderVorbis : public Encoder {
public:
    EncoderVorbis(EncoderConfig c = EncoderConfig());
    ~EncoderVorbis(void);
    bool init();

    void setup();
    void encode(sample_t* buffer, uint32_t samples);
    void finalize();

    QString getVersion() const;
    QString getFileExtension() const;

private:
    void handleRC(int rc) const;
    void writeHeader();
    void resize(uint32_t newsize);

private:
    ogg_stream_state _oss;
    ogg_page _opg;
    ogg_packet _opk;
    vorbis_info _vi;
    vorbis_comment _vc;
    vorbis_block _vb;
    vorbis_dsp_state _vdsp;
};

#endif
