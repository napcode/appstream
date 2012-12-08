#include "filelogger.h"

FileLogger* FileLogger::_instance = 0;

FileLogger& FileLogger::instance()
{
    if (!_instance)
    {
        _instance = new FileLogger;
    }
    return *_instance;
}
void FileLogger::release()
{
	delete _instance;
}
FileLogger::FileLogger()
:_f(0)
{
    _f = fopen("output.log", "w+");
}
FileLogger::~FileLogger()
{
    if (_f)
    {
        fclose(_f);
        _f = 0;
    }
}
void FileLogger::log(const sample_t *buffer, uint32_t samples, uint8_t channels)
{
    QMutexLocker ml(&_m);
    assert(_f);
    for (uint32_t i = 0; i < samples; i += channels)
    {
        if (channels == 1)
            fprintf(_f, "%d\n", buffer[i]);
        else if (channels == 2)
            fprintf(_f, "%d,%d\n", buffer[i], buffer[i + 1]);
    }
    fflush(_f);
}
