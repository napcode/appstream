#include "outputfile.h"
#include "logger.h"

#include <QDateTime>
#include <QDate>
#include <QSettings>

#include <iostream>

OutputFile::OutputFile(QString path, QString filename)
 :	_path(path),
	_filename(filename)
{
    connect(this, SIGNAL(message(QString)), Logger::getInstance(), SLOT(log(QString)));    
	parseFileName();
}
OutputFile::~OutputFile()
{
	if(_file.isOpen()) {
		_file.flush();
		_file.close();
		if(_file.size() == 0)
			_file.remove();
	}
}
bool OutputFile::init()
{
	if(_encoder) {
		_filename.append(".");
		_filename.append(_encoder->getFileExtension());		
	}
	if(!_path.endsWith('/'))
		_path.append('/');
	_file.setFileName(_path + _filename);
	_file.open(QIODevice::WriteOnly);
	if(!_file.isOpen()) {
		emit message("Error: unable to open file " + _path + _filename);
		return false;
	}
    if(!_file.isWritable()) {
		emit message("Error: file " + _path + _filename + " is not writeable");
		return false;
	}
	emit message("Recording to " + _path + _filename);
	start();
	return true;
}
void OutputFile::output(const char *buffer, uint32_t size)
{    
    if(!_file.isOpen() || !_file.isWritable())
		return;
	_file.write(buffer, size);
}
void OutputFile::parseFileName()
{
	QSettings s;
	s.beginGroup("stream");
	QString f(_filename);
	if(f.contains("%date%"))
		f.replace("%date%", getDateString());
	if(f.contains("%streamname%") && s.contains("selected")) {
		QString sname = s.value("selected").toString();
		sname = sname.simplified();
		sname = sname.replace(' ','_');
		sname = sname.replace(':','_');
		sname = sname.replace('/','_');
		f.replace("%streamname%", sname);
	}
	_filename = f;
}
QString OutputFile::getDateString() const
{
    QDateTime dt = QDateTime::currentDateTime();
	QString res;
	res += QString::number(dt.date().year());
	int v = dt.date().month();
	res += QString("%1").arg(v,2,10,QChar('0'));
	v = dt.date().day();
	res += QString("%1").arg(v,2,10,QChar('0'));
	res += "-";
	v = dt.time().hour();
	res += QString("%1").arg(v,2,10,QChar('0'));
	v = dt.time().minute();
	res += QString("%1").arg(v,2,10,QChar('0'));

	return res;
}