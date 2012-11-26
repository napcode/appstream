#include "mainwindow.h"
#include "settingsdialog.h"
#include "ui_mainwindow.h"
#include <QAction>
#include <QDateTime>

#include "audiosystem.h"
#include "dsp.h"
#include "config.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _dsp = new DSP(2);
    // setup settings system
    QCoreApplication::setOrganizationName("apparatus");
    QCoreApplication::setOrganizationDomain("apparatus.de");
    QCoreApplication::setApplicationName("appStream");

    connect(ui->mainToolBar,SIGNAL(actionTriggered(QAction*)),this,SLOT(toolbarTriggered(QAction*)));

    AudioSystem::Manager &as = AudioSystem::Manager::getInstance();
    // whenever there is a state change in the audio manager we'd like to log it
    connect(&as, SIGNAL(stateChanged(QString)), this, SLOT(log(QString)));
    connect(&as, SIGNAL(newAudioFrames()), this, SLOT(updateVUMeter()), Qt::QueuedConnection);    
    connect(_dsp, SIGNAL(stateChanged(QString)), this, SLOT(log(QString)), Qt::QueuedConnection);;
    
    _dsp->start();
    as.init();
    as.setDSP(_dsp);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete _dsp;
}
void MainWindow::toolbarTriggered(QAction *a)
{
    QString text;
    if(a == ui->actionSettings) {
        SettingsDialog s;
        s.exec();
    }
    else if (a == ui->actionStartStream) {
        AudioSystem::Manager &as = AudioSystem::Manager::getInstance();
        as.openDeviceStream();
    }
    else if (a == ui->actionStopStream) {
        AudioSystem::Manager &as = AudioSystem::Manager::getInstance();
        as.closeDeviceStream();
    }
}
void MainWindow::log(QString s)
{
    QDateTime current = QDateTime::currentDateTime();
    QString entry = current.toString(Qt::SystemLocaleShortDate);
    entry += QString(": ") + s;
    ui->logLabel->appendPlainText(entry);
}
void MainWindow::updateVUMeter()
{
    //_dsp->test();
    //const sample_t *p = _dsp->getPeaks();
    //log(QString::number(p[0]) + QString(", ") + QString::number(p[1]));
}
