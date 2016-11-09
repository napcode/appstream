#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <stdint.h>
#include "logger.h"
#include "fxeditor.h"
#include "types.h"

namespace Ui {
class MainWindow;
}

class DSP;
class Encoder;

class MainWindow : public Logger {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

    void start();
    void stop();
public slots:
    void toolbarTriggered(QAction* a);
    void message(QString s);
    void warn(QString s);
    void error(QString s);
    void newAudioFrames(float timestamp, uint32_t frames);
    void setStreamInfo(QString);

private:
    void prepareDSP(uint8_t channels, uint32_t samplerate);
    void addFileRecorder();
    void addStream();
    Encoder* constructEncoder(const QSettings& s) const;
    void toggleRecording();
    void toggleStreaming();
    void updateStreamSettings();

    Ui::MainWindow* ui;
    DSP* _dsp;
    FXEditor* _fxeditor;
    AudioSystem::ManagerPtr _manager;
};

#endif // MAINWINDOW_H
