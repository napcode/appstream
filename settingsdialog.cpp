#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "audiosystem.h"
#include "serverconnectiondialog.h"
#include "streaminfodialog.h"
#include <QSettings>
#include <QMessageBox>
#include <QFileDialog>
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
    connect(ui->rmConnection, SIGNAL(released()), this, SLOT(rmConnection()));
    connect(ui->addStreamInfo, SIGNAL(released()), this, SLOT(addStreamInfo()));
    connect(ui->editStreamInfo, SIGNAL(released()), this, SLOT(editStreamInfo()));
    connect(ui->rmStreamInfo, SIGNAL(released()), this, SLOT(rmStreamInfo()));
	connect(ui->slEncoderBitrate, SIGNAL(sliderMoved(int)),this,SLOT(sliderMoved(int)));
	connect(ui->toolButton, SIGNAL(clicked()), this, SLOT(openFileDialog()));

    /* load settings */
    applySettings();
}
SettingsDialog::~SettingsDialog()
{
    delete ui;
}
void SettingsDialog::applySettings()
{
    QSettings s;
    s.beginGroup("general");
    if (s.contains("connectOnStart"))
        ui->cbConnectAtStart->setChecked(s.value("connectOnStart").toBool());
    s.endGroup();
    /* trigger refresh */
    updateAudioDeviceList(-1);
    applyAudioSettings();
    applyRecordSettings();
    applyConnectionSettings();
    applyStreamSettings();
}
void SettingsDialog::applyAudioSettings()
{
    QSettings s;
    s.beginGroup("audio");
    if (s.contains("sampleRate"))
    {
        int i = ui->cbSampleRate->findText(s.value("sampleRate").toString());
        ui->cbSampleRate->setCurrentIndex(i);
    }
    if (s.contains("bitsPerSample"))
    {
        int i = ui->cbBitsPerSample->findText(s.value("bitsPerSample").toString());
        ui->cbBitsPerSample->setCurrentIndex(i);
    }
    if (s.contains("numChannels"))
    {
        int i = ui->cbChannels->findText(s.value("numChannels").toString());
        ui->cbChannels->setCurrentIndex(i);
    }
    if (s.contains("deviceName"))
    {
        int i = ui->cbInputDevices->findText(s.value("deviceName").toString());
        ui->cbInputDevices->setCurrentIndex(i);
    }
    s.endGroup();
}
void SettingsDialog::applyRecordSettings()
{
	QSettings s;
	s.beginGroup("record");
	if(s.contains("encoder")) {
		int i = ui->cbEncoder->findText(s.value("encoder").toString());
		ui->cbEncoder->setCurrentIndex(i);
	}
	if(s.contains("encoderBitRate"))
		ui->slEncoderBitrate->setValue(s.value("encoderBitRate").toInt());
	if(s.contains("encoderSampleRate")) {
		int i = ui->cbEncoderSamplerate->findText(s.value("encoderSampleRate").toString());
		ui->cbEncoderSamplerate->setCurrentIndex(i);
	}
	if(s.contains("recordPath"))
		ui->leRecordPath->setText(s.value("recordPath").toString());
	if(s.contains("recordFileName"))
		ui->leRecordFilename->setText(s.value("recordFileName").toString());
	s.endGroup();
}
void SettingsDialog::applyConnectionSettings()
{
    ui->cbConnection->clear();
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
    ui->cbStreamInfo->clear();
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
    {
        s.beginGroup("general");
        s.setValue("connectOnStart", ui->cbConnectAtStart->isChecked());
        s.endGroup();
    }
    {
        /* store record settings here */
		s.beginGroup("record");
		s.setValue("encoder", ui->cbEncoder->currentText());
		s.setValue("encoderBitRate", ui->slEncoderBitrate->value());
		s.setValue("encoderSampleRate", ui->cbEncoderSamplerate->currentText());
		s.setValue("recordPath", ui->leRecordPath->text());
		s.setValue("recordFileName", ui->leRecordFilename->text());
		s.endGroup();
    }
    this->done(QDialog::Accepted);
}
void SettingsDialog::updateAudioDeviceList(int)
{
    AudioSystem::Mode m;
    ui->cbInputDevices->clear();

    m.sampleRate = ui->cbSampleRate->currentText().toInt();
    m.bitsPerSample = ui->cbBitsPerSample->currentText().toInt();
    m.numChannels = ui->cbChannels->currentText().toInt();
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
    ServerConnectionDialog scd;
    int ret = scd.exec();
    if (ret == QDialog::Accepted)
    {
        applyConnectionSettings();
    }
}
void SettingsDialog::editConnection()
{
    if (ui->cbConnection->currentText().isNull() || ui->cbConnection->currentText().isEmpty())
        return;
    ServerConnectionDialog scd;
    scd.setConnection(ui->cbConnection->currentText());
    scd.exec();
}
void SettingsDialog::rmConnection()
{
    if (ui->cbConnection->currentText().isNull() || ui->cbConnection->currentText().isEmpty())
        return;
    QMessageBox m(QMessageBox::Warning, "Delete connection?", "Delete the selected connection?", QMessageBox::Yes | QMessageBox::No );
    m.setDefaultButton(QMessageBox::No);
    int ret = m.exec();
    if (ret != QMessageBox::Yes)
        return;
    QSettings s;
    s.beginGroup("connection");
    s.remove("selected");
    s.beginGroup(ui->cbConnection->currentText());
    s.remove("");
    s.endGroup();
    s.endGroup();
    int i = ui->cbConnection->currentIndex();
    ui->cbConnection->removeItem(i);
}
void SettingsDialog::addStreamInfo()
{
    StreamInfoDialog sid;
    int ret = sid.exec();
    if (ret == QDialog::Accepted)
    {
        applyStreamSettings();
    }
}
void SettingsDialog::editStreamInfo()
{
    if (ui->cbStreamInfo->currentText().isNull() || ui->cbStreamInfo->currentText().isEmpty())
        return;
    StreamInfoDialog sid;
    sid.setStreamInfo(ui->cbStreamInfo->currentText());
    sid.exec();
}
void SettingsDialog::rmStreamInfo()
{
    if (ui->cbStreamInfo->currentText().isNull() || ui->cbStreamInfo->currentText().isEmpty())
        return;
    QMessageBox m(QMessageBox::Warning, "Delete stream?", "Delete the selected stream?", QMessageBox::Yes | QMessageBox::No );
    m.setDefaultButton(QMessageBox::No);
    int ret = m.exec();
    if (ret != QMessageBox::Yes)
        return;
    QSettings s;
    s.beginGroup("stream");
    s.remove("selected");
    s.beginGroup(ui->cbStreamInfo->currentText());
    s.remove("");
    s.endGroup();
    s.endGroup();
    int i = ui->cbStreamInfo->currentIndex();
    ui->cbStreamInfo->removeItem(i);
}
void SettingsDialog::sliderMoved(int position)
{
	ui->slEncoderBitrate->setToolTip(QString::number(position));
}
void SettingsDialog::openFileDialog()
{
	QString dir = QFileDialog::getExistingDirectory(this,"Select Directory");
	ui->leRecordPath->setText(dir);
}