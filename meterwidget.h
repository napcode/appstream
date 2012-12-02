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
public slots:
    void setValues(MeterValues m);
protected:
    void paintEvent(QPaintEvent *event);
private:
	inline float clamp(sample_t x, sample_t a, sample_t b)
	{
	    return x < a ? a : (x > b ? b : x);
	}
    uint8_t _numChannels;
    MeterValues _v;
    float _width;
    float _colorspan;
};

#endif // VUWIDGET_H
