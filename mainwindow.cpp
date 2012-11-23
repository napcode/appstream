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

    // setup settings system
    QCoreApplication::setOrganizationName("apparatus");
    QCoreApplication::setOrganizationDomain("apparatus.de");
    QCoreApplication::setApplicationName("appStream");

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
        SettingsDialog s;
        s.exec();
    }
    else if (a == ui->actionStartStream) {
        AudioSystem::Manager &as = AudioSystem::Manager::getInstance();
        as.openDeviceStream();
    }
    else if (a = ui->actionStopStream) {
        AudioSystem::Manager &as = AudioSystem::Manager::getInstance();
        as.closeDeviceStream();
    }
}
void MainWindow::log(QString s)
{
    QDateTime current = QDateTime::currentDateTime();
    QString entry = current.toString(Qt::SystemLocaleShortDate);
    entry += QString(": ") + s;
    ui->logLabel->appendPlainText(entry);
}
