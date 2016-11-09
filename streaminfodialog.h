#ifndef STREAMINFODIALOG_H
#define STREAMINFODIALOG_H

#include <QDialog>

namespace Ui {
class StreamInfoDialog;
}

class StreamInfoDialog : public QDialog {
    Q_OBJECT

public:
    explicit StreamInfoDialog(QWidget* parent = 0);
    ~StreamInfoDialog();

    void setStreamInfo(const QString& name);
public slots:
    void accept();

private:
    Ui::StreamInfoDialog* ui;
};

#endif // STREAMINFODIALOG_H
