#include "meterwidget.h"
#include <QPainter>
#include <QPaintEvent>
#include <iostream>
#include <cmath>

MeterWidget::MeterWidget(QWidget *parent, uint8_t channels)
    : QLabel(parent)
{
    _colorspan = 120.0f/360.0f;
    setNumChannels(channels);
}
MeterWidget::~MeterWidget()
{

}
void MeterWidget::setNumChannels(uint8_t channels)
{
    _numChannels = channels;
    _v.resize(channels);
    _width = width() / channels;
    reset();
}
void MeterWidget::reset()
{
    MeterValues::iterator it = _v.begin();
    while(it != _v.end()) {
        *it = 0;
        it++;
    }
    update();
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
        rect.setRect(i*w , h - h*_v[i], w-1, h*_v[i]);
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
    update();
}
