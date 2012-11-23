#ifndef SERVERCONNECTIONSDIALOG_H
#define SERVERCONNECTIONSDIALOG_H

#include <QDialog>

namespace Ui {
class ServerConnectionDialog;
}

class ServerConnectionDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ServerConnectionDialog(QWidget *parent = 0);
    ~ServerConnectionDialog();

    void setConnection(QString name);

public slots:
    void accept();
    
private:
    Ui::ServerConnectionDialog *ui;
};

#endif // SERVERCONNECTIONSDIALOG_H
