#ifndef FILELOGGER_H
#define FILELOGGER_H

#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include <QMutex>
#include <QMutexLocker>

#include "config.h"

class FileLogger
{
public:
	static FileLogger& instance();	
	static void release();	
	void log(const sample_t *buffer, uint32_t samples, uint8_t channels);

	private:
		FileLogger();
		~FileLogger();

		static FileLogger *_instance;
		QMutex _m;
		FILE *_f;
};

#endif