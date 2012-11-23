#include "serverconnectiondialog.h"
#include "ui_serverconnectiondialog.h"
#include <QSettings>
#include <QMessageBox>

ServerConnectionDialog::ServerConnectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ServerConnectionDialog)
{
    ui->setupUi(this);
}

ServerConnectionDialog::~ServerConnectionDialog()
{
    delete ui;
}
void ServerConnectionDialog::setConnection(QString name)
{
	QSettings s;
	QString base("connection/");
	if(s.contains(base+name)) {
		QString tmp(base+name+"/");
		if(s.contains(tmp + QString("type"))) {
			int i = ui->cbType->findText(s.value(tmp + QString("type")).toString());
			ui->cbType->setCurrentIndex(i);
		}
	}

}
void ServerConnectionDialog::accept()
{
	if (ui->edName->text().isEmpty() || ui->edName->text().isNull()) {
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
   		s.setValue("adress", ui->edAdress->text());
   		s.setValue("port", ui->sbPort->value());
   		s.setValue("password", ui->edPassword->text());
   		s.setValue("mountpoint", ui->edMountpoint->text());
   	}
    s.endGroup();
    s.endGroup();


    // close & delete dialog
    delete this;
}