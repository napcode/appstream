#include "meterwidget.h"
#include <QPainter>

MeterWidget::MeterWidget(short channels)
    :_numChannels(channels)
{
    _values = new short[_numChannels];
}
MeterWidget::~MeterWidget()
{
    delete[] _values;
}
void MeterWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.setPen(Qt::green);
    painter.drawText(10, 10, QString::number(_values[0]));
}
