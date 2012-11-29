#include "meterwidget.h"
#include <QPainter>
#include <QPaintEvent>
#include <iostream>
#include <cmath>

MeterWidget::MeterWidget(QWidget *parent, uint8_t channels)
    : QLabel(parent), _numChannels(channels)
{
    _values = new sample_t[_numChannels];
    for(uint8_t i = 0; i < _numChannels; ++i)
        _values[i] = 0;
    _width = width() / _numChannels;
    _colorspan = 120.0f/360.0f;
}
MeterWidget::~MeterWidget()
{
    delete[] _values;
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
        rect.setRect(i*w , h - h*_values[i], w, h*_values[i]);
        float col = _colorspan - (_colorspan * _values[i]);
        color.setHsvF(col, 1.0f, 1.0f);
        QBrush b(color);
        painter.setBrush(b);
        painter.drawRect(rect);
    }
}
void MeterWidget::setValues(sample_t l, sample_t r)
{  
    _values[0] = l;
    _values[1] = r;
    update();
}
