#include "mainwindow.h"
#include "settingsdialog.h"
#include "ui_mainwindow.h"
#include <QAction>
#include <QDateTime>

#include "audiosystem.h"
#include "dsp.h"
#include "meterprocessor.h"
#include "compressorprocessor.h"
#include "config.h"
#include "encoderlame.h"
#include "encodervorbis.h"
#include "outputfile.h"
#include "outputicecast.h"
#include "aboutdialog.h"

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
    _fxeditor = new FXEditor(this);
    _fxeditor->setShown(false);
    // create & start DSP
    _dsp = new DSP();
    as.setDSP(_dsp);
    connect(_dsp, SIGNAL(newPeaks(MeterValues)), ui->meterwidget, SLOT(setValues(MeterValues)));

    QSettings s;
    s.beginGroup("record");
    if(s.contains("enabled")) {
        ui->actionRecord->setChecked(s.value("enabled").toBool());
    }
    s.endGroup();
    s.beginGroup("connection");
    if(s.contains("enabled")) {
        ui->actionStreaming->setChecked(s.value("enabled").toBool());
    }
    s.endGroup();
    s.beginGroup("general");
    if(s.contains("connectOnStart") && s.value("connectOnStart").toBool()) {
        start();
    }
    if(s.contains("windowpos_x") && s.contains("windowpos_y")) {
        this->move(s.value("windowpos_x").toInt(), s.value("windowpos_y").toInt());
    }
    s.endGroup();
    updateStreamSettings();
    connect(ui->cbStreamInfo, SIGNAL(currentIndexChanged(QString)), this, SLOT(setStreamInfo(QString)));
}

MainWindow::~MainWindow()
{
    AudioSystem::Manager &as = AudioSystem::Manager::getInstance();
    if(as.getState() == AudioSystem::Manager::STREAMING)
        stop();
    QSettings s;
    s.beginGroup("general");
    s.setValue("windowpos_x", this->x());
    s.setValue("windowpos_y", this->y());
    s.endGroup();
}
void MainWindow::toolbarTriggered(QAction *a)
{
    QString text;
    if(a == ui->actionSettings) {
        SettingsDialog s;
        s.exec();
        updateStreamSettings();
    } else if (a == ui->actionRun) {
        if(ui->actionRun->isChecked())
            start();
        else
            stop();
    } else if (a == ui->actionRecord) {
        toggleRecording();
    } else if (a == ui->actionStreaming) {
        toggleStreaming();
    } else if (a == ui->actionEditFX) {
        _fxeditor->setFloating(true);
        _fxeditor->show();
    } else if (a == ui->actionAbout) {
        AboutDialog a;
        a.exec();
    }
}
void MainWindow::toggleRecording()
{
    QSettings s;
    s.beginGroup("record");
    s.setValue("enabled",ui->actionRecord->isChecked());
    s.endGroup();
    if(_dsp->isActive()) {
        if(ui->actionRecord->isChecked()) {
            addFileRecorder();
        } else {
            OutputList::iterator it = _dsp->getOutputList().begin();
            while(it != _dsp->getOutputList().end()) {
                if((*it)->getName() == "SELECTED_RECORDER") {
                    Output *o = *it;
                    o->disable();
                    disconnect(o,SIGNAL(stateChanged(QString)), ui->statuswidget, SLOT(setRecorderState(QString)));
                    _dsp->removeOutput(*it);
                    ui->statuswidget->stopRecording();
                    break;
                }
                it++;
            }
        }
    }
}
void MainWindow::toggleStreaming()
{
    QSettings s;
    s.beginGroup("connection");
    s.setValue("enabled",ui->actionStreaming->isChecked());
    s.endGroup();

    if(_dsp->isActive()) {
        if(ui->actionStreaming->isChecked()) {
            addStream();
        } else {
            OutputList::iterator it = _dsp->getOutputList().begin();
            while(it != _dsp->getOutputList().end()) {
                if((*it)->getName() == "SELECTED_STREAM") {
                    Output *o = *it;
                    o->disable();
                    disconnect(o,SIGNAL(stateChanged(QString)), ui->statuswidget, SLOT(setStreamState(QString)));
                    _dsp->removeOutput(o);
                    ui->statuswidget->stopStreaming();
                    ui->cbStreamInfo->setEnabled(true);
                    break;
                }
                it++;
            }
        }
    }
}
void MainWindow::start()
{
    AudioSystem::Manager &as = AudioSystem::Manager::getInstance();
    if(as.getState() != AudioSystem::Manager::INITIALIZED)
        return;

    QSettings s;
    s.beginGroup("audio");
    if (!s.contains("numChannels")) {
        error("Configuration invalid");
        return;
    }

    if(as.openDeviceStream()) {
        prepareDSP(as.getCurrentMode().numChannels, as.getCurrentMode().sampleRate);
        ui->meterwidget->setNumChannels(as.getCurrentMode().numChannels);

        _dsp->start();
        as.startDeviceStream();
        ui->meterwidget->toggleActive(true);
        ui->actionRun->setChecked(true);
    }
}
void MainWindow::stop()
{
    AudioSystem::Manager &as = AudioSystem::Manager::getInstance();
    if(as.getState() != AudioSystem::Manager::STREAMING)
        return;

    as.stopDeviceStream();
    as.closeDeviceStream();
    _dsp->disable();
    _dsp->reset();
    ui->meterwidget->toggleActive(false);
    ui->meterwidget->reset();
    ui->statuswidget->stopRecording();
    ui->statuswidget->stopStreaming();
    ui->cbStreamInfo->setEnabled(true);
}
void MainWindow::message(QString s)
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
    entry += QString(": *Warn* ") + s;
    ui->logLabel->appendPlainText(entry);
}
void MainWindow::error(QString s)
{
    QDateTime current = QDateTime::currentDateTime();
    QString entry = current.toString(Qt::SystemLocaleShortDate);
    entry += QString(": *Error* ") + s;
    ui->logLabel->appendPlainText(entry);
}
void MainWindow::newAudioFrames(float ts, uint32_t frames)
{
    //_dsp->test();
    //const sample_t *p = _dsp->getPeaks();
    //log(QString::number(ts) + QString("::") + QString::number(frames));
}

void MainWindow::prepareDSP(uint8_t channels, uint32_t samplerate)
{
    _dsp->setNumChannels(channels);
    MeterProcessor *mp = new MeterProcessor(channels, samplerate);

    CompressorProcessor *cp = new CompressorProcessor(channels, samplerate);
    _fxeditor->connectWithProcessor(*cp);
    _dsp->addProcessor(cp);
    _dsp->addProcessor(mp);
    if(ui->actionRecord->isChecked()) {
        addFileRecorder();
    }
    if(ui->actionStreaming->isChecked()) {
        addStream();
    }
}
void MainWindow::addStream()
{
    QSettings s;
    s.beginGroup("connection");
    if(!s.contains("selected")) {
        emit warn("no valid connection selected");
        return;
    }
    QString name = s.value("selected").toString();
    s.beginGroup(name);

    OutputIceCast *oic = 0;
    Encoder *e = 0;
    if(!s.contains("address") || !s.contains("port") ||
            !s.contains("user") || !s.contains("password") ||
            !s.contains("mountpoint") || !s.contains("encoder") ||
            !s.contains("encoderQuality") || !s.contains("encoderSampleRate") ||
            !s.contains("encoderChannels") || !s.contains("protocol")) {
        error("erroneous stream config");
        return;
    }

    e = constructEncoder(s);
    if(!e)
        return;

    oic = new OutputIceCast;
    oic->setName("SELECTED_STREAM");
    oic->setEncoder(e);
    oic->setConnection(name);
    if(!oic->init()) {
        error("unable to init stream");
        delete oic;
        return;
    }
    connect(oic,SIGNAL(stateChanged(QString)), ui->statuswidget, SLOT(setStreamState(QString)));
    {
        QSettings s;
        s.beginGroup("stream");
        if(s.contains("selected"))
            oic->setStreamInfo(s.value("selected").toString());
    }

    ui->statuswidget->startStreaming();
    ui->cbStreamInfo->setDisabled(true);
    oic->connectStream();
    _dsp->addOutput(oic);
    oic->start();
}
void MainWindow::addFileRecorder()
{
    QSettings s;
    s.beginGroup("record");
    OutputFile *f = 0;
    Encoder *e = 0;
    if(!s.contains("recordPath") || !s.contains("recordFileName") ||
            !s.contains("encoder") || !s.contains("encoderQuality") ||
            !s.contains("encoderSampleRate")) {
        error("erroneous recorder config");
        return;
    }

    e = constructEncoder(s);
    if(!e)
        return;

    f = new OutputFile(s.value("recordPath").toString(),
                       s.value("recordFileName").toString());
    f->setEncoder(e);
    connect(f, SIGNAL(stateChanged(QString)), ui->statuswidget, SLOT(setRecorderState(QString)));
    if(!f->init()) {
        error("unable to init recorder");
        disconnect(f, SIGNAL(stateChanged(QString)), ui->statuswidget, SLOT(setRecorderState(QString)));
        delete f;
        return;
    }
    f->setName("SELECTED_RECORDER");

    ui->statuswidget->startRecording();
    _dsp->addOutput(f);
    f->start();
}
Encoder* MainWindow::constructEncoder(const QSettings &s) const
{
    Encoder *e;
    EncoderConfig c;
    AudioSystem::Manager &as = AudioSystem::Manager::getInstance();
    c.sampleRateOut = s.value("encoderSampleRate").toInt();
    c.numInChannels = _dsp->getNumChannels();
    c.sampleRateIn = as.getCurrentMode().sampleRate;
    QString mode = s.value("encoderMode").toString();
    if(mode == "CBR")
        c.mode = EncoderConfig::CBR;
    else if (mode == "VBR")
        c.mode = EncoderConfig::VBR;
    else
        c.mode = EncoderConfig::CBR;

    c.quality = s.value("encoderQuality").toFloat();

    if(s.value("encoder").toString() == QString("Lame MP3"))
        e = new EncoderLame(c);
    else if(s.value("encoder").toString() == QString("Ogg Vorbis"))
        e = new EncoderVorbis(c);

    assert(e);
    if(!e->init()) {
        delete e;
        return 0;
    }
    return e;
}
void MainWindow::updateStreamSettings()
{
    ui->cbStreamInfo->clear();
    QSettings s;
    s.beginGroup("stream");
    QStringList connections = s.childGroups();
    QStringListIterator it(connections);
    while (it.hasNext()) {
        ui->cbStreamInfo->addItem(it.next());
    }
    // is there a selected entry?
    if (s.contains("selected")) {
        int i = ui->cbStreamInfo->findText(s.value("selected").toString());
        ui->cbStreamInfo->setCurrentIndex(i);
    }
}
void MainWindow::setStreamInfo(QString name)
{
    QSettings s;
    s.beginGroup("stream");
    s.setValue("selected", name);
    s.endGroup();
}
