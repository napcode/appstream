#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <stdint.h>

namespace Ui {
class MainWindow;

}
class DSP;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void toolbarTriggered(QAction *a);
    void log(QString s);
    void newAudioFrames(float timestamp, uint32_t frames);
    
private:
    Ui::MainWindow *ui;
    DSP *_dsp;
};

#endif // MAINWINDOW_H
