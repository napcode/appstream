#ifndef VUWIDGET_H
#define VUWIDGET_H

#include <QLabel>

class VUWidget : public QLabel
{
    Q_OBJECT
public:
    VUWidget(short channels = 1);
    ~VUWidget();
protected:
    void paintEvent(QPaintEvent *event);
private:
    short _numChannels;
    short *_values;
};

#endif // VUWIDGET_H
