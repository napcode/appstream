#include "outputfile.h"

#include <QDateTime>
#include <QDate>
#include <QSettings>

#include <iostream>

OutputFile::OutputFile(QString path, QString filename)
: Output(),
  _path(path),
  _filename(filename),
  _written(0)
{
}
OutputFile::~OutputFile()
{
    if (_file.isOpen()) {
        _file.flush();
        _file.close();
        if (_file.size() == 0)
            _file.remove();
    }
}
bool OutputFile::init()
{
    parseFileName();
    QString ext;
    if (_encoder) {
        ext.append(".");
        ext.append(_encoder->getFileExtension());
    }
    if (!_path.endsWith('/'))
        _path.append('/');

    _file.setFileName(_path + _filename + ext);
    int i = 0;
    while (_file.exists()) {
        if (i == 0)
            emit warn("file already exists.");
        ++i;
        _file.setFileName(_path + _filename + "-" + QString::number(i) + ext);
    }

    _file.open(QIODevice::WriteOnly);
    if (!_file.isOpen()) {
        emit error("unable to open file " + _path + _filename);
        return false;
    }
    if (!_file.isWritable()) {
        emit error("file " + _path + _filename + " is not writeable");
        return false;
    }
    emit message("Recording to " + _path + _filename + ext);
    emit stateChanged("active");
    start();
    return true;
}
void OutputFile::disable()
{
    Output::disable();
    emit stateChanged("inactive");
}
void OutputFile::output(const char* buffer, uint32_t size)
{
    if (size == 0 || !_file.isOpen() || !_file.isWritable())
        return;
    qint64 written = _file.write(buffer, size);
    if (_written == -1) {
        emit error(_file.errorString());
        emit stateChanged("error");
        return;
    }
    _written += written;
    if (_written > 1000000) {
        // force write to disk
        // in case of crashes or problems we'd like to write regularly
        _written = 0;
        _file.flush();

        // flush doesn't really force a write :(
        _file.close();
        _file.open(QIODevice::Append);
    }
}
void OutputFile::parseFileName()
{
    QSettings s;
    s.beginGroup("stream");
    QString f(_filename);
    if (f.contains("%date%"))
        f.replace("%date%", getDateString());
    if (f.contains("%streamname%") && s.contains("selected")) {
        QString sname = s.value("selected").toString();
        sname = sname.simplified();
        sname = sname.replace(' ', '_');
        sname = sname.replace(':', '_');
        sname = sname.replace('/', '_');
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
    res += QString("%1").arg(v, 2, 10, QChar('0'));
    v = dt.date().day();
    res += QString("%1").arg(v, 2, 10, QChar('0'));
    res += "-";
    v = dt.time().hour();
    res += QString("%1").arg(v, 2, 10, QChar('0'));
    v = dt.time().minute();
    res += QString("%1").arg(v, 2, 10, QChar('0'));

    return res;
}