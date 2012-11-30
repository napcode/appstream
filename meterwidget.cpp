#include "meterwidget.h"
#include <QPainter>
#include <QPaintEvent>
#include <iostream>
#include <cmath>

MeterWidget::MeterWidget(QWidget *parent, uint8_t channels)
    : QLabel(parent), _numChannels(channels)
{
    _v.resize(_numChannels);
    _width = width() / _numChannels;
    _colorspan = 120.0f/360.0f;
}
MeterWidget::~MeterWidget()
{

}

void MeterWidget::paintEvent(QPaintEvent *event)
{
    QRect r = event->rect();
    int w = r.width() / _numChannels;
    int h = r.height();
    QRect rect;
    QColor color;
    QPainter painter(this);
    for (uint8_t i=0; i < _numChannels; ++i) {
        rect.setRect(i*w , h - h*_v[i], w, h*_v[i]);
        float col = _colorspan - (_colorspan * _v[i]);

        color.setHsvF(col, 1.0f, 1.0f);
        QBrush b(color);
        painter.setBrush(b);
        painter.drawRect(rect);
    }
}
void MeterWidget::setValues(MeterValues m)
{
    _v = m;
    //_values[0] = clamp(l, 0.0f, 1.0f);
   // _values[1] = clamp(r, 0.0f, 1.0f);
    update();
}
