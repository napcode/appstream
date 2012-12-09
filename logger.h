#ifndef LOGGER_H
#define LOGGER_H
#include <QString>
#include <QMainWindow>

// kinda hacky
// wrapper to add this definition to all classes and to ease up logging
class Logger : public QMainWindow
{
    Q_OBJECT
public:
    Logger(QWidget *parent)
        : QMainWindow(parent)
    {}
    static Logger* getInstance() { return _instance; }
public slots:
    virtual void message(QString msg) = 0;
    virtual void warn(QString msg) = 0;
    virtual void error(QString msg) = 0;
protected:
    static Logger* _instance;
};


#endif // LOGGER_H
