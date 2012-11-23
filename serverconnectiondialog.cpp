#include "serverconnectiondialog.h"
#include "ui_serverconnectiondialog.h"

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
