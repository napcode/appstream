#include "mainwindow.h"
#include "settingsdialog.h"
#include "ui_mainwindow.h"
#include <QAction>
#include <QDateTime>

#include "audiosystem.h"

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->mainToolBar,SIGNAL(actionTriggered(QAction*)),this,SLOT(toolbarTriggered(QAction*)));
    AudioSystem::Manager &as = AudioSystem::Manager::getInstance();
    connect(&as, SIGNAL(stateChanged(QString)), this, SLOT(log(QString)));
    as.init();
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::toolbarTriggered(QAction *a)
{
    QString text;
    if(a == ui->actionSettings) {
        text.append("click");
        SettingsDialog *s = new SettingsDialog;
        s->show();
        //delete s;
    } else
        text.append("asdfjakösdj");
    ui->loglabel->setText(text);
}
void MainWindow::log(QString s)
{
    QDateTime current = QDateTime::currentDateTime();
    QString entry = current.toString();
    entry += QString(": ") + s + QString("\n");
    QString old = ui->loglabel->text();
    old.append(entry);
    ui->loglabel->setText(old);
}
