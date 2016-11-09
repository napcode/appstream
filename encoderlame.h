#ifndef ENCODERLAME_H
#define ENCODERLAME_H

#include "encoder.h"

#include <stdint.h>

#include <lame/lame.h>

class EncoderLame : public Encoder {
public:
    EncoderLame(EncoderConfig c = EncoderConfig());
    ~EncoderLame();

    // reimplemented from Encoder
    bool init();
    // reimplemented from Encoder
    void setup();
    void encode(sample_t* buffer, uint32_t samples);
    void finalize();

    // reimplemented from Encoder
    QString getVersion() const;
    QString getFileExtension() const { return QString("mp3"); }
private:
    void handleRC(int rc) const;
    void resize(uint32_t newSize);

private:
    lame_global_flags* _lgf;
    uint32_t _allocedFrames;
};

#endif // ENCODERLAME_H
