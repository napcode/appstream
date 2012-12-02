#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <stdint.h>
#include "logger.h"

namespace Ui {
class MainWindow;

}
class DSP;

class MainWindow : public Logger
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void startStream();
    void stopStream();
public slots:
    void toolbarTriggered(QAction *a);
    void log(QString s);
    void warn(QString s);
    void error(QString s);
    void newAudioFrames(float timestamp, uint32_t frames);
    
private:
    Ui::MainWindow *ui;
    DSP *_dsp;
};

#endif // MAINWINDOW_H
