#include "serverconnectiondialog.h"
#include "ui_serverconnectiondialog.h"
#include <QSettings>
#include <QMessageBox>

ServerConnectionDialog::ServerConnectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ServerConnectionDialog)
{
    ui->setupUi(this);
    connect(ui->cbType, SIGNAL(currentIndexChanged(QString)), this, SLOT(typeChanged(QString)));
}

ServerConnectionDialog::~ServerConnectionDialog()
{
    delete ui;
}
void ServerConnectionDialog::typeChanged(QString current)
{
    QSettings s;
    s.beginGroup("connection");
    if (current == "Icecast")
    {
        ui->edUser->setEnabled(true);
        ui->edMountpoint->setEnabled(true);
        if (s.contains("user"))
            ui->edUser->setText(s.value("user").toString());
        if (s.contains("mountpoint"))
            ui->edUser->setText(s.value("mountpoint").toString());
    }
    else if (current == "Shoutcast")
    {
        ui->edUser->setEnabled(false);
        ui->edUser->setText("source");
        ui->edMountpoint->setEnabled(false);
        ui->edMountpoint->setText("/stream");
    }
}
void ServerConnectionDialog::setConnection(const QString &name)
{
    QSettings s;
    s.beginGroup("connection");
    s.beginGroup(name);
    ui->edName->setText(name);
    ui->edName->setEnabled(false);
    if (s.contains("address"))
    {
        ui->edAddress->setText(s.value("address").toString());
    }
    if (s.contains("port"))
    {
        ui->sbPort->setValue(s.value("port").toInt());
    }
    if (s.contains("user"))
    {
        ui->edUser->setText(s.value("user").toString());
    }
    if (s.contains("password"))
    {
        ui->edPassword->setText(s.value("password").toString());
    }
    if (s.contains("mountpoint"))
    {
        ui->edMountpoint->setText(s.value("mountpoint").toString());
    }
    if (s.contains("encoder"))
    {
        int i = ui->cbEncoder->findText(s.value("encoder").toString());
        ui->cbEncoder->setCurrentIndex(i);
    }
    if (s.contains("encoderBitRate"))
    {
        ui->slEncoderBitRate->setValue(s.value("encoderBitRate").toInt());
    }
    if (s.contains("encoderSampleRate"))
    {
        int i = ui->cbEncoderSampleRate->findText(s.value("encoderSampleRate").toString());
        ui->cbEncoderSampleRate->setCurrentIndex(i);
    }
    if (s.contains("encoderChannels"))
    {
        int i = ui->cbEncoderChannels->findText(s.value("encoderChannels").toString());
        ui->cbEncoderChannels->setCurrentIndex(i);
    }
    if (s.contains("type"))
    {
        int i = ui->cbType->findText(s.value("type").toString());
        ui->cbType->setCurrentIndex(i);
        typeChanged(ui->cbType->currentText());
    }

    s.endGroup();
    s.endGroup();
}
void ServerConnectionDialog::accept()
{
    if (ui->edName->text().isEmpty() || ui->edName->text().isNull())
    {
        QMessageBox b;
        b.setText("Name can't be empty");
        b.exec();
        return;
    }

    QSettings s;
    s.beginGroup("connection");
    s.beginGroup(ui->edName->text());
    {
        s.setValue("type", ui->cbType->currentText());
        s.setValue("address", ui->edAddress->text());
        s.setValue("port", ui->sbPort->value());
        s.setValue("user", ui->edUser->text());
        s.setValue("password", ui->edPassword->text());
        s.setValue("mountpoint", ui->edMountpoint->text());
        s.setValue("encoder", ui->cbEncoder->currentText());
        s.setValue("encoderBitRate", ui->slEncoderBitRate->value());
        s.setValue("encoderSampleRate", ui->cbEncoderSampleRate->currentText());
        s.setValue("encoderChannels", ui->cbEncoderChannels->currentText());
    }
    s.endGroup();
    s.endGroup();
    this->done(QDialog::Accepted);
}
