#include "meterwidget.h"
#include <QPainter>
#include <QPaintEvent>
#include <iostream>

MeterWidget::MeterWidget(QWidget *parent, short channels)
    : QLabel(parent), _numChannels(channels)
{
    _values = new sample_t[_numChannels];
}
MeterWidget::~MeterWidget()
{
    delete[] _values;
}

void MeterWidget::paintEvent(QPaintEvent *event)
{
    QRect r = event->rect();
    int w = r.width();
    int h = r.height();
    QRect left(0 , h - h*_values[0], w>>1, h*_values[0]);
    QRect right(w>>1 , h - h*_values[1], w>>1, h*_values[1]);
    QColor cleft,cright;

    float colorspan = 120.0/360.0;
    float cl = colorspan - (colorspan * _values[0]);
    float cr = colorspan - (colorspan * _values[1]);

    cleft.setHsvF(cl, 1.0f, 1.0f);
    cright.setHsvF(cr, 1.0f, 1.0f);
    
    QPainter painter(this);
    QBrush b(cleft);
    painter.setBrush(b); 

    painter.drawRect(left);
    b.setColor(cright);
    painter.drawRect(right);
}
void MeterWidget::setValues(sample_t l, sample_t r)
{  
    _values[0] = clamp(log10(100*l), 0.0f, 1.0f);
    _values[1] = clamp(log10(100*r), 0.0f, 1.0f);
    update();
}
