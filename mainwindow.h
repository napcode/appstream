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

    void start();
    void stop();
public slots:
    void toolbarTriggered(QAction *a);
    void message(QString s);
    void warn(QString s);
    void error(QString s);
    void newAudioFrames(float timestamp, uint32_t frames);

    
private:
    void prepareDSP(uint8_t channels);
    void addFileRecorder();
    void addStream();
    Ui::MainWindow *ui;
    DSP *_dsp;
};

#endif // MAINWINDOW_H
