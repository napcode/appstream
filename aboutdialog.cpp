#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget* parent)
: QDialog(parent),
  ui(new Ui::aboutdialog)
{
    ui->setupUi(this);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
