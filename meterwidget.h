#ifndef VUWIDGET_H
#define VUWIDGET_H

#include <QLabel>

class MeterWidget : public QLabel
{
    Q_OBJECT
public:
    MeterWidget(short channels = 1);
    ~MeterWidget();
protected:
    void paintEvent(QPaintEvent *event);
private:
    short _numChannels;
    short *_values;
};

#endif // VUWIDGET_H
