#include "meterwidget.h"
#include <QPainter>
#include <QPaintEvent>
#include <QAction>
#include <QMenu>
#include <iostream>
#include <cmath>

MeterWidget::MeterWidget(QWidget* parent, uint8_t channels)
: QLabel(parent),
  _isActive(true)
{
    _colorspan = 120.0f / 360.0f;
    setNumChannels(channels);
    QAction* act_p = new QAction("Toggle Metering", this);
    act_p->setCheckable(true);
    connect(act_p, SIGNAL(toggled(bool)), this, SLOT(toggleActive(bool)));
    this->addAction(act_p);
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
    for (uint8_t i = 0; i < _v.size(); ++i) {
        _v[i] = 0;
    }
    update();
}

void MeterWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    QRect r = event->rect();
    QColor color;
    color.setRgb(0, 0, 0);
    int w = r.width() / _v.size();
    int h = r.height();
    // draw outer frame
    QBrush b(color);
    QRect border(0, 0, r.width() - 1, r.height() - 1);
    painter.setBrush(b);
    painter.drawRect(border);

    if (!_isActive)
        return;

    QRect rect;

    for (uint8_t i = 0; i < _v.size(); ++i) {
        rect.setRect(i * w, h - h * _v[i], w - 1, h * _v[i]);
        float col = _colorspan - (_colorspan * _v[i]);

        color.setHsvF(col, 1.0f, 1.0f);
        QBrush b(color);
        painter.setBrush(b);
        painter.drawRect(rect);
    }
}
void MeterWidget::setValues(MeterValues m)
{
    if (_isActive) {
        _v = m;
        update();
    }
}
