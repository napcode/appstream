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
    void addConnection();
    void editConnection();
    void delConnection();
    void editStreamInfos();
    void accept();
    
private:
	void applySettings();
	void applyConnectionSettings();
	void applyStreamSettings();

    Ui::SettingsDialog *ui;

private slots:
    void updateAudioDeviceList(int dummy);
};

#endif // SETTINGSDIALOG_H
