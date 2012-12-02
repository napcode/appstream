#include "outputfile.h"
#include "logger.h"

#include <iostream>

OutputFile::OutputFile()
 : _filename("output")
{
    connect(this, SIGNAL(message(QString)), Logger::getInstance(), SLOT(log(QString)));    
}
OutputFile::~OutputFile()
{
	if(_file.isOpen()) {
		_file.flush();
		_file.close();
	}
}
bool OutputFile::init()
{
	if(_encoder) {
		_filename.append(".");
		_filename.append(_encoder->getFileExtension());		
	}
	_file.setFileName(_filename);
	_file.open(QIODevice::WriteOnly);
	if(!_file.isOpen()) {
		emit message("Error: unable to open file " + _filename);
		return false;
	}
    if(!_file.isWritable()) {
		emit message("Error: file " + _filename + " is not writeable");
		return false;
	}
	start();
	return true;
}
void OutputFile::output(const char *buffer, uint32_t bufferSize)
{    
    if(!_file.isOpen() || !_file.isWritable())
		return;
	_file.write(buffer, bufferSize);
}
