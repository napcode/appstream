#ifndef SERVERCONNECTIONSDIALOG_H
#define SERVERCONNECTIONSDIALOG_H

#include <QDialog>

namespace Ui {
class ServerConnectionDialog;
}

class ServerConnectionDialog : public QDialog {
    Q_OBJECT

public:
    explicit ServerConnectionDialog(QWidget* parent = 0);
    ~ServerConnectionDialog();

    void setConnection(const QString& name);

public slots:
    void accept();
    void typeChanged(QString);
    void updateEncoderMode(int);
    void qualitySpinBoxChanged(int);
    void qualitySliderChanged(int);

private:
    Ui::ServerConnectionDialog* ui;
};

#endif // SERVERCONNECTIONSDIALOG_H
