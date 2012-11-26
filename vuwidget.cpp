#include "vuwidget.h"
#include <QPainter>

VUWidget::VUWidget(short channels)
    :_numChannels(channels)
{
    _values = new short[_numChannels];
}
VUWidget::~VUWidget()
{
    delete[] _values;
}
void VUWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.setPen(Qt::green);
    painter.drawText(10, 10, QString::number(_values[0]));
}
