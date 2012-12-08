#ifndef CONFIG_H
#define CONFIG_H

#define DSP_BLOCKSIZE 256
#define DSP_RINGSIZE 2048
#define PA_FRAMES 64
#define OUTPUT_RINGSIZE 2048

typedef short sample_t;

#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include <QMutex>

static FILE *f = 0;
static QMutex m;
static void filelog(const sample_t *buffer, uint32_t samples, uint8_t channels)
{
	m.lock();
	if(!f) {
		f = fopen("output.log", "w+");
	}

	for(uint32_t i = 0; i < samples; i+=channels) {
		if(channels == 1) 
			fprintf(f, "%d\n", buffer[i]);
		else if(channels == 2) 
			fprintf(f, "%d,%d\n", buffer[i], buffer[i+1]);
	}
	fflush(f);
	m.unlock();
}
static void fileclose()
{
	if(f) {
		fclose(f);
		f = 0;
	}
}

#endif
