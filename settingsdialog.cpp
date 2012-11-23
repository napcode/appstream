#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "audiosystem.h"
#include "serverconnectiondialog.h"
#include <QSettings>
#include <iostream>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    /* connect signals & slots */
    connect(ui->cbSampleRate, SIGNAL(currentIndexChanged(int)), this, SLOT(updateAudioDeviceList(int)));
    connect(ui->cbBitsPerSample, SIGNAL(currentIndexChanged(int)), this, SLOT(updateAudioDeviceList(int)));
    connect(ui->cbChannels, SIGNAL(currentIndexChanged(int)), this, SLOT(updateAudioDeviceList(int)));
    connect(ui->addConnection, SIGNAL(released()), this, SLOT(addConnection()));
    connect(ui->editConnection, SIGNAL(released()), this, SLOT(editConnection()));
    connect(ui->delConnection, SIGNAL(released()), this, SLOT(delConnection()));

    /* load settings */
    applySettings();

    /* trigger refresh */
    updateAudioDeviceList(-1);
}
SettingsDialog::~SettingsDialog()
{
    delete ui;
}
void SettingsDialog::applySettings()
{
    QSettings s;
    if (s.contains("audio/sampleRate"))
    {
        int i = ui->cbSampleRate->findText(s.value("audio/sampleRate").toString());
        ui->cbSampleRate->setCurrentIndex(i);
    }
    if (s.contains("audio/bitsPerSample"))
    {
        int i = ui->cbBitsPerSample->findText(s.value("audio/bitsPerSample").toString());
        ui->cbBitsPerSample->setCurrentIndex(i);
    }
    if (s.contains("audio/numChannels"))
    {
        int i = ui->cbChannels->findText(s.value("audio/numChannels").toString());
        ui->cbChannels->setCurrentIndex(i);
    }
    if (s.contains("audio/deviceName"))
    {
        int i = ui->cbInputDevices->findText(s.value("audio/deviceName").toString());
        ui->cbInputDevices->setCurrentIndex(i);
    }
    applyConnectionSettings();
    applyStreamSettings();
}
void SettingsDialog::applyConnectionSettings()
{
    QSettings s;
    s.beginGroup("connection");
    QStringList connections = s.childGroups();
    QStringListIterator it(connections);
    while (it.hasNext())
    {
        ui->cbConnection->addItem(it.next());
    }
    if (s.contains("selected"))
    {
        int i = ui->cbConnection->findText(s.value("selected").toString());
        ui->cbConnection->setCurrentIndex(i);
    }
}
void SettingsDialog::applyStreamSettings()
{
    QSettings s;
    s.beginGroup("stream");
    QStringList connections = s.childGroups();
    QStringListIterator it(connections);
    while (it.hasNext())
    {
        ui->cbStreamInfo->addItem(it.next());
    }
    // is there a selected entry?
    if (s.contains("selected"))
    {
        int i = ui->cbStreamInfo->findText(s.value("selected").toString());
        ui->cbStreamInfo->setCurrentIndex(i);
    }
}
void SettingsDialog::accept()
{
    QSettings s;
    {
        s.beginGroup("audio");
        s.setValue("sampleRate", ui->cbSampleRate->currentText());
        s.setValue("bitsPerSample", ui->cbBitsPerSample->currentText());
        s.setValue("numChannels", ui->cbChannels->currentText());
        s.setValue("deviceName", ui->cbInputDevices->currentText());
        s.endGroup();
    }
    {
        s.beginGroup("connection");
        s.setValue("selected", ui->cbConnection->currentText());
        s.endGroup();
    }
    {
        s.beginGroup("stream");
        s.setValue("selected", ui->cbStreamInfo->currentText());
        s.endGroup();
    }
    // close & delete dialog
    delete this;
}
void SettingsDialog::updateAudioDeviceList(int)
{
    AudioSystem::Mode m;
    ui->cbInputDevices->clear();

    m.sampleRate = ui->cbSampleRate->currentText().toInt();
    m.bitsPerSample = ui->cbBitsPerSample->currentText().toInt();
    if (ui->cbChannels->currentText() == QString("Mono"))
        m.numChannels = 1;
    else
        m.numChannels = 2;
    AudioSystem::Manager &as = AudioSystem::Manager::getInstance();
    AudioSystem::DeviceList dl = as.getDeviceList();
    AudioSystem::DeviceList::Iterator it = dl.begin();
    while (it != dl.end())
    {
        if (as.checkModeSupported(*it, m))
            ui->cbInputDevices->addItem(it->first);
        ++it;
    }
}
void SettingsDialog::addConnection()
{
    ServerConnectionDialog *scd = new ServerConnectionDialog;
    scd->show();
}
void SettingsDialog::editConnection()
{
    ServerConnectionDialog *scd = new ServerConnectionDialog;
    scd->setConnection(ui->cbConnection->currentText());
    scd->show();
}
void SettingsDialog::delConnection()
{

}
void SettingsDialog::editStreamInfos()
{

}
