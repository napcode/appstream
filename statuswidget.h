#ifndef STATUSWIDGET_H
#define STATUSWIDGET_H

#include <QFrame>
#include <QTimer>
#include <QTime>

namespace Ui {
class StatusWidget;
}

class StatusWidget : public QFrame {
    Q_OBJECT

public:
    explicit StatusWidget(QWidget* parent = 0);
    ~StatusWidget();
public slots:
    void startRecording();
    void stopRecording();
    void startStreaming();
    void stopStreaming();
    void updateTime();
    void setStreamState(QString state);
    void setRecorderState(QString state);
    void blink();

private:
    Ui::StatusWidget* ui;
    QString msToString(int ms);
    QTimer* _timer;
    QTimer* _blinkTimer;
    bool _isRecording;
    bool _isStreaming;
    QTime _timeRec;
    QTime _timeStream;
    QString _timeFormat;
    bool _blinkState;
    QString _styleNormal;
    QString _styleHightlight;
};

#endif // STATUSWIDGET_H
