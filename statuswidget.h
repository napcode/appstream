#ifndef STATUSWIDGET_H
#define STATUSWIDGET_H

#include <QFrame>

class StatusWidget : public QFrame
{
    Q_OBJECT
public:
    explicit StatusWidget(QWidget *parent = 0);
signals:
    
public slots:

protected:
    void paintEvent(QPaintEvent *event);
};

#endif // STATUSWIDGET_H
