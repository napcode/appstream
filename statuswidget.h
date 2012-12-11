#ifndef STATUSWIDGET_H
#define STATUSWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QTime>

namespace Ui {
class StatusWidget;
}

class StatusWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit StatusWidget(QWidget *parent = 0);
    ~StatusWidget();

public slots:
	void startRecording();
	void stopRecording();
	void startStreaming();
	void stopStreaming();
	void updateTime();
private:
    Ui::StatusWidget *ui;
    QString msToString(int ms);
	QTimer *_timer;
	bool _isRecording;
	bool _isStreaming;
	QTime _timeRec;
	QTime _timeStream;
	QString _timeFormat;
};

#endif // STATUSWIDGET_H
