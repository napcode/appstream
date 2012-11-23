#include "streaminfodialog.h"
#include "ui_streaminfodialog.h"
#include <QSettings>
#include <QMessageBox>

StreamInfoDialog::StreamInfoDialog(QWidget *parent) :
QDialog(parent),
ui(new Ui::StreamInfoDialog)
{
	ui->setupUi(this);
}

StreamInfoDialog::~StreamInfoDialog()
{
	delete ui;
}
void StreamInfoDialog::setStreamInfo(const QString &name)
{
	QSettings s;
	s.beginGroup("stream");
	s.beginGroup(name);
	ui->edName->setText(name);
	ui->edName->setReadOnly(true);
	if(s.contains("genre")) {
		ui->edGenre->setText(s.value("genre").toString());	
	}
	if(s.contains("description")) {
		ui->edDescription->setText(s.value("description").toString());
	}
	if(s.contains("url")) {
		ui->edUrl->setText(s.value("url").toString());
	}
	if(s.contains("icq")) {
		ui->edIcq->setText(s.value("icq").toString());
	}
	if(s.contains("irc")) {
		ui->edIrc->setText(s.value("irc").toString());
	}
	s.endGroup();
	s.endGroup();
}
void StreamInfoDialog::accept()
{
	if (ui->edName->text().isEmpty() || ui->edName->text().isNull()) {
		QMessageBox b;
		b.setText("Name can't be empty");
		b.exec();		
		return;
	}

	QSettings s;
    s.beginGroup("stream");
    s.beginGroup(ui->edName->text());
   	{
        s.setValue("genre", ui->edGenre->text());
        s.setValue("description", ui->edDescription->text());
   		s.setValue("url", ui->edUrl->text());
   		s.setValue("icq", ui->edIcq->text());
   		s.setValue("irc", ui->edIrc->text());
   	}
    s.endGroup();
    s.endGroup();
    this->done(QDialog::Accepted);
}
