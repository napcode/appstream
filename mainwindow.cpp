#include "mainwindow.h"
#include "settingsdialog.h"
#include "ui_mainwindow.h"
#include <QAction>
#include <QDateTime>
#include <QSettings>

#include "audiosystem.h"
#include "dsp.h"
#include "meterprocessor.h"
#include "config.h"
#include "filelogger.h"

MainWindow::MainWindow(QWidget *parent) :
    Logger(parent),   
    ui(new Ui::MainWindow), 
    _dsp(0)
{
    ui->setupUi(this);
    // needed for logger
    _instance = this;
    AudioSystem::Manager &as = AudioSystem::Manager::getInstance();
    as.init();
    // setup settings system
    QCoreApplication::setOrganizationName("apparatus");
    QCoreApplication::setOrganizationDomain("apparatus.de");
    QCoreApplication::setApplicationName("appStream");

    connect(ui->mainToolBar,SIGNAL(actionTriggered(QAction*)),Logger::getInstance(),SLOT(toolbarTriggered(QAction*)));
    qRegisterMetaType<uint32_t>("uint32_t");
    qRegisterMetaType<sample_t>("sample_t");
    qRegisterMetaType<MeterValues>("MeterValues");
    // get audio system
    //AudioSystem::Manager &as = AudioSystem::Manager::getInstance();
    // whenever there is a state change in the audio manager we'd like to log it
    //connect(&as, SIGNAL(message(QString)), Logger::getInstance(), SLOT(log(QString)));
    //as.init();
    QSettings s;
    s.beginGroup("record");
    if(s.contains("enabled")) {
        ui->actionRecord->setChecked(s.value("enabled").toBool());
    }

}

MainWindow::~MainWindow()
{
    if(_dsp)
        stopStream();
    FileLogger::release();
}
void MainWindow::toolbarTriggered(QAction *a)
{
    QString text;
    if(a == ui->actionSettings) {
        SettingsDialog s;
        s.exec();
    }
    else if (a == ui->actionStartStream) {
        startStream();
    }
    else if (a == ui->actionStopStream) {
        stopStream();
    }
    else if (a == ui->actionRecord) {
        QSettings s;
        s.beginGroup("record");
        s.setValue("enabled",ui->actionRecord->isChecked());
        s.endGroup();
		if(_dsp)
			_dsp->addFileRecorder();
    }
}
void MainWindow::startStream()
{
	if(_dsp)
		return;
    // FIXME check for a valid config before doing anything
    QSettings s;
    s.beginGroup("audio");
    if (!s.contains("numChannels"))
    {
        log("Error: please review the configuration.");
        return;
    }
    uint8_t channels = s.value("numChannels").toInt();
    // get audio system
    AudioSystem::Manager &as = AudioSystem::Manager::getInstance();
    ui->meterwidget->setNumChannels(channels);
  //  connect(_dsp, SIGNAL(message(QString)), Logger::getInstance(), SLOT(log(QString)), Qt::QueuedConnection);
   // connect(_dsp, SIGNAL(newPeaks(MeterValues)), ui->meterwidget, SLOT(setValues(MeterValues)));
        
    _dsp = new DSP(channels);
	_dsp->defaultSetup();
	_dsp->start();
	as.setDSP(_dsp);
    if(as.openDeviceStream()) {	   
	    // FIXME this should be based on settings
	}
    else {
		as.setDSP(0);
        delete _dsp;
		_dsp = 0;
	}
}
void MainWindow::stopStream()
{
    if(!_dsp)
        return;
    AudioSystem::Manager &as = AudioSystem::Manager::getInstance();
    as.closeDeviceStream();
    _dsp->disable();
    //disconnect(_dsp, SIGNAL(message(QString)), Logger::getInstance(), SLOT(log(QString)));
    //disconnect(_dsp, SIGNAL(newPeaks(MeterValues)), ui->meterwidget, SLOT(setValues(MeterValues)));
    as.setDSP(0);
    delete _dsp;
    _dsp = 0;
    ui->meterwidget->reset();
}
void MainWindow::log(QString s)
{
    QDateTime current = QDateTime::currentDateTime();
    QString entry = current.toString(Qt::SystemLocaleShortDate);
    entry += QString(": ") + s;
    ui->logLabel->appendPlainText(entry);
}
void MainWindow::warn(QString s)
{
    QDateTime current = QDateTime::currentDateTime();
    QString entry = current.toString(Qt::SystemLocaleShortDate);
    entry += QString(": ") + s;
    ui->logLabel->appendPlainText(entry);
}
void MainWindow::error(QString s)
{
    QDateTime current = QDateTime::currentDateTime();
    QString entry = current.toString(Qt::SystemLocaleShortDate);
    entry += QString(": ") + s;
    ui->logLabel->appendPlainText(entry);
}
void MainWindow::newAudioFrames(float ts, uint32_t frames)
{
    //_dsp->test();
    //const sample_t *p = _dsp->getPeaks();
    //log(QString::number(ts) + QString("::") + QString::number(frames));
}

