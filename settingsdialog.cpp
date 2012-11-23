#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "audiosystem.h"
#include "serverconnectiondialog.h"
#include <QMessageBox>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    connect(ui->cbSampleRate,SIGNAL(currentIndexChanged(int)),this,SLOT(updateAudioDeviceList(int)));
    connect(ui->cbBitsPerSample,SIGNAL(currentIndexChanged(int)),this,SLOT(updateAudioDeviceList(int)));
    connect(ui->cbChannels,SIGNAL(currentIndexChanged(int)),this,SLOT(updateAudioDeviceList(int)));
    updateAudioDeviceList(-1);
}
SettingsDialog::~SettingsDialog()
{
    delete ui;
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
    AudioSystem::Manager& as = AudioSystem::Manager::getInstance();
    AudioSystem::DeviceList dl = as.getDeviceList();
    AudioSystem::DeviceList::Iterator it = dl.begin();
    while(it != dl.end()) {
        if(as.checkModeSupported(*it, m))
            ui->cbInputDevices->addItem(it->first);
        ++it;
    }
    QMessageBox a;
    a.show();
}
void SettingsDialog::editServerConnections()
{
    ServerConnectionDialog *scd = new ServerConnectionDialog;
    scd->show();
}
void SettingsDialog::editStreamInfos()
{

}
