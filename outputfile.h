#ifndef OUTPUTFILE_H
#define OUTPUTFILE_H

#include "output.h"
#include <QString>
#include <QFile>

class OutputFile : public Output
{
    Q_OBJECT
public:
    OutputFile(QString path = QString(), QString filename = QString("output"));
    ~OutputFile();

    // reimplementation from Output
    bool init();
    void disable();
    
    void output(const char *buffer, uint32_t size);

    const QString &getFileName() const
    {
        return _filename;
    }
    void setFileName(const QString &filename)
    {
        _filename = filename;
    }
signals:
    //void stateChanged(State);
    void stateChanged(QString);
protected:
	QString _path;
    QString _filename;
    QFile _file;
    uint64_t _written;

private:
	void parseFileName();
	QString getDateString() const;
};

#endif // OUTPUTFILE_H
