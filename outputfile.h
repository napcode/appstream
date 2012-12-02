#ifndef OUTPUTFILE_H
#define OUTPUTFILE_H

#include "output.h"
#include <QString>
#include <QFile>

class OutputFile : public Output
{
public:
    OutputFile();
    ~OutputFile();

    // reimplementation from Output
    bool init();
    void output(const char *buffer, uint32_t size);

    const QString &getFileName() const
    {
        return _filename;
    }
    void setFileName(const QString &filename)
    {
        _filename = filename;
    }
protected:
    QString _filename;
    QFile _file;
};

#endif // OUTPUTFILE_H
