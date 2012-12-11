#include "statuswidget.h"
#include "ui_statuswidget.h"

StatusWidget::StatusWidget(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::StatusWidget),
    _timer(new QTimer(this)),
    _isRecording(false),
    _isStreaming(false),
    _timeFormat("hh:mm:ss")
{
    ui->setupUi(this);
    _timer->setSingleShot(false);
	_timer->setInterval(1000);		
	connect(_timer, SIGNAL(timeout()), this, SLOT(updateTime()));
	updateTime();
    _timer->start();
    ui->lbRecord->setText("inactive");
    ui->lbStream->setText("offline");
}

StatusWidget::~StatusWidget()
{
    delete ui;
    delete _timer;
}
void StatusWidget::updateTime()
{
	QString time;
	int ms;
    QString tmp("00:00:00");
	{
        if(_isStreaming) {
            ms = _timeStream.elapsed();
            time = msToString(ms);
        }
        else
            time = tmp;
        ui->lbStreamDuration->setText(time);
    }
    {
        if(_isRecording) {
            ms = _timeRec.elapsed();
            time = msToString(ms);
        }
        else
            time = tmp;
        ui->lbRecordDuration->setText(time);
    }
    update();
}
void StatusWidget::startRecording()
{
	_timeRec.start();
	_isRecording = true;
	//ui->lbRecord->setText("active");
	update();
}
void StatusWidget::stopRecording()
{
	_isRecording = false;
	//ui->lbRecord->setText("inactive");
	update();
}
void StatusWidget::startStreaming()
{
	_timeStream.start();
	_isStreaming = true;
	//ui->lbStream->setText("online");
	update();
}
void StatusWidget::stopStreaming()
{
	_isStreaming = false;
	//ui->lbStream->setText("offline");
	update();
}
void StatusWidget::setRecorderState(QString state)
{
	ui->lbRecord->setText(state);
}
void StatusWidget::setStreamState(QString state)
{
	ui->lbStream->setText(state);
}
QString StatusWidget::msToString(int ims)
{
    int h,m,s,ms;
    h = ims/(1000*60*60);
    ims -= h*(1000*60*60);
    m = ims/(1000*60);
    ims -= m*(1000*60);
    s = ims/(1000);
    ms = ims-s*(1000);
    QString t("%1:%2:%3");
    t = t.arg(h,2,10,QChar('0'));
    t = t.arg(m,2,10,QChar('0'));
    t = t.arg(s,2,10,QChar('0'));
    //t.arg(h,2,10,'0');
    return t;
}