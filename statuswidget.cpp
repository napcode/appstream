#include "statuswidget.h"
#include <QBrush>
#include <QPainter>
#include <QPaintEvent>

StatusWidget::StatusWidget(QWidget *parent) :
    QFrame(parent)
{
}
void StatusWidget::paintEvent(QPaintEvent *event)
{
    QRect r = event->rect();

    QPainter painter(this);
    QBrush b(Qt::red);
    painter.setBrush(b);
    painter.drawRect(r);
}
