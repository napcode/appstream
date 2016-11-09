#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include "audiosystem.h"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(const AudioSystem::Manager&, QWidget* parent = 0);
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
    AudioSystem::SAMPLEFORMAT getSampleFormat(const QString& text) const;

    const AudioSystem::Manager& _manager;
    Ui::SettingsDialog* ui;

private slots:
    void updateAudioDeviceList(int);
    void updateEncoderMode(int);
    void qualitySpinBoxChanged(int);
    void qualitySliderChanged(int);
};

#endif // SETTINGSDIALOG_H
