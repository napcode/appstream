#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

public slots:
    void editServerConnections();
    void editStreamInfos();
    
private:
    Ui::SettingsDialog *ui;
private slots:
    void updateAudioDeviceList(int dummy);
};

#endif // SETTINGSDIALOG_H
