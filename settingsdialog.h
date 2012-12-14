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
    void rmConnection();
    void addStreamInfo();
    void editStreamInfo();
    void rmStreamInfo();
    void accept();
	void openFileDialog();
    
private:
    void applySettings();
    void applyAudioSettings();
    void applyRecordSettings();
    void applyConnectionSettings();
    void applyStreamSettings();

    Ui::SettingsDialog *ui;

private slots:
    void updateAudioDeviceList(int);
    void updateEncoderMode(int);
    void qualitySpinBoxChanged(int);
    void qualitySliderChanged(int);
};

#endif // SETTINGSDIALOG_H
