#ifndef VUWIDGET_H
#define VUWIDGET_H

#include <QLabel>
#include "config.h"
#include <stdint.h>

class MeterWidget : public QLabel
{
    Q_OBJECT
public:
    MeterWidget(QWidget *parent, uint8_t channels = 2);
    ~MeterWidget();
public slots:
    void setValues(sample_t l, sample_t r);
protected:
    void paintEvent(QPaintEvent *event);
private:
	inline float clamp(sample_t x, sample_t a, sample_t b)
	{
	    return x < a ? a : (x > b ? b : x);
	}
    short _numChannels;
    sample_t *_values;
    float _width;
    float _colorspan;
};

#endif // VUWIDGET_H
