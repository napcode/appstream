#ifndef CONFIG_H
#define CONFIG_H

#define DSP_BLOCKSIZE 512
#define DSP_RINGSIZE 2048
#define PA_FRAMES 256
#define OUTPUT_RINGSIZE 2048
#define PROJECT_NAME "appStream"

// NOTE:
// lame encoder currently assumes interleaved doubles!
typedef double sample_t;

#endif
