#ifndef VUWIDGET_H
#define VUWIDGET_H

#include <QLabel>
#include "config.h"
#include <stdint.h>
#include "meterprocessor.h"

class MeterWidget : public QLabel
{
    Q_OBJECT
public:
    MeterWidget(QWidget *parent, uint8_t channels = 1);
    ~MeterWidget();

    uint8_t getNumChannels() const { return _numChannels; }
    void setNumChannels(uint8_t channels);
    void reset();
    bool isActive() const { return _isActive; }
    void setActive(bool active) { _isActive = active; }
public slots:
    void setValues(MeterValues m);
protected:
    void paintEvent(QPaintEvent *event);
private:
    bool _isActive;
    uint8_t _numChannels;
    MeterValues _v;
    float _width;
    float _colorspan;
};

#endif // VUWIDGET_H
