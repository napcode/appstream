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
void ServerConnectionDialog::setConnection(const QString &name)
{
	QSettings s;
  s.beginGroup("connection");
	s.beginGroup(name);
  ui->edName->setText(name);
  ui->edName->setReadOnly(true);
	if(s.contains("type")) {
    int i = ui->cbType->findText(s.value("type").toString());
		ui->cbType->setCurrentIndex(i);		
	}
  if(s.contains("adress")) {
    ui->edAdress->setText(s.value("adress").toString());
  }
  if(s.contains("port")) {
    ui->sbPort->setValue(s.value("port").toInt());
  }
  if(s.contains("password")) {
    ui->edPassword->setText(s.value("password").toString());
  }
  if(s.contains("mountpoint")) {
    ui->edMountpoint->setText(s.value("mountpoint").toString());
  }
  s.endGroup();
  s.endGroup();
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
    this->done(QDialog::Accepted);
}
