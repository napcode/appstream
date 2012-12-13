#include "outputicecast.h"
#include "logger.h"

#include <QSettings>


OutputIceCast::OutputIceCast()
:	_shout(0),
	_state(INVALID),
    _timer(new QTimer(this))
{
    _timer->setInterval(5000);
    _timer->setSingleShot(false);
    connect(_timer, SIGNAL(timeout()), this, SLOT(connectStream()));
    connect(this, SIGNAL(requestReconnect()), this, SLOT(reconnectStream()), Qt::QueuedConnection);
    shout_init();
}
OutputIceCast::~OutputIceCast()
{
    if(getState() == CONNECTED)
        disconnectStream();
	shout_free(_shout);
	shout_shutdown();
}
void OutputIceCast::disable()
{
    Output::disable();
    if(getState() == CONNECTED)
        disconnectStream();
    if(_timer->isActive())
        _timer->stop();
}
void OutputIceCast::setConnection(const QString &name)
{
    QSettings s;
    s.beginGroup("connection");
    s.beginGroup(name);

    _c.address = s.value("address").toString();
    _c.port = s.value("port").toInt();
    _c.user = s.value("user").toString();
    _c.password = s.value("password").toString();
    _c.mountpoint = s.value("mountpoint").toString();
    _c.encoder = s.value("encoder").toString();
}
void OutputIceCast::setConnection(const ConfigConnection& config)
{
    _c = config;
}
void OutputIceCast::setStreamInfo(const QString &name)
{
    QSettings s;
    s.beginGroup("stream");
    s.beginGroup(name);

    _csi.name = s.value("name").toString();
    _csi.description = s.value("description").toInt();
    _csi.agent = s.value("agent").toString();
    _csi.genre = s.value("genre").toString();
    _csi.url = s.value("url").toString();
    _csi.isPublic = s.value("isPublic").toBool();
}
void OutputIceCast::setStreamInfo(const ConfigStreamInfo &config)
{
    _csi = config;
}
bool OutputIceCast::init()
{
	//shout_init();
	_shout = shout_new();
	if(!_shout)
		return false;

	int r;
    r = shout_set_host(_shout, _c.address.toStdString().c_str());
    if(r != SHOUTERR_SUCCESS) {
        emit error(QString("unable to set hostname: ") + shout_get_error(_shout));
    	return false;    	
    }
    r = shout_set_protocol(_shout, SHOUT_PROTOCOL_HTTP);
    if(r != SHOUTERR_SUCCESS) {
        emit error(QString("unable to set protocol: ") + shout_get_error(_shout));
    	return false;    	
    }
    r = shout_set_port(_shout, _c.port);
    if(r != SHOUTERR_SUCCESS) {
        emit error(QString("unable to set port: ") + shout_get_error(_shout));
    	return false;    	
    }
    r = shout_set_mount(_shout, _c.mountpoint.toStdString().c_str());
    if(r != SHOUTERR_SUCCESS) {
       emit error(QString("unable to set mountpoint: ") + shout_get_error(_shout));
    	return false;    	
    }
    r = shout_set_user(_shout, _c.user.toStdString().c_str());
    if(r != SHOUTERR_SUCCESS) {
        emit error(QString("unable to set user: ") + shout_get_error(_shout));
    	return false;    	
    }
    r = shout_set_password(_shout, _c.password.toStdString().c_str());
    if(r != SHOUTERR_SUCCESS) {
        emit error(QString("unable to set password: ") + shout_get_error(_shout));
    	return false;    	
    }
    if(_c.encoder == QString("Lame MP3"))
    	r = shout_set_format(_shout, SHOUT_FORMAT_MP3);
    else if(_c.encoder == QString("Ogg Vorbis"))
    	r = shout_set_format(_shout, SHOUT_FORMAT_OGG);
    if(r != SHOUTERR_SUCCESS) {
        emit error(QString("unable to set protocol: ") + shout_get_error(_shout));
    	return false;    	
    }

    _state = READY;
    emit stateChanged(READY);
    emit stateChanged("ready");
    emit message("libshout initialized");
    emit message("Version: " + getVersion());
	return true;
}
void OutputIceCast::applyStreamInfo()
{
	int r;
    r = shout_set_name(_shout, _csi.name.toStdString().c_str());
    if(r != SHOUTERR_SUCCESS) {
        emit error(QString("unable to set name: ") + shout_get_error(_shout)); 	
    }
    r = shout_set_genre(_shout, _csi.genre.toStdString().c_str());
    if(r != SHOUTERR_SUCCESS) {
        emit error(QString("unable to set genre: ") + shout_get_error(_shout));  
    }
    r = shout_set_url(_shout, _csi.url.toStdString().c_str());
    if(r != SHOUTERR_SUCCESS) {
        emit error(QString("unable to set url: ") + shout_get_error(_shout));  
    }
    r = shout_set_agent(_shout, _csi.agent.toStdString().c_str());
    if(r != SHOUTERR_SUCCESS) {
        emit error(QString("unable to set agent: ") + shout_get_error(_shout));  
    }
    r = shout_set_description(_shout, _csi.description.toStdString().c_str());
    if(r != SHOUTERR_SUCCESS) {
        emit error(QString("unable to set description: ") + shout_get_error(_shout));  
    }
    r = shout_set_public(_shout, static_cast<int>(_csi.isPublic));
    if(r != SHOUTERR_SUCCESS) {
        emit error(QString("unable to set description: ") + shout_get_error(_shout));  
    }
    // TODO set metadata
    //shout_set_metadata
}
void OutputIceCast::connectStream()
{
	if(getState() != READY && getState() != DISCONNECTED)
		return;
    emit stateChanged("connecting");
    int r = shout_open(_shout);
    if(r == SHOUTERR_SUCCESS) {
    	_state = CONNECTED;
    	emit stateChanged(CONNECTED);
        emit stateChanged("connected");
        emit message(QString("Connected to ") + _c.address);
        if(_timer->isActive())
            _timer->stop();
    }
    else {
        if(_timer->isActive()) {
            ++_trial;
            
        }
        emit warn(shout_get_error(_shout));
    	_state = DISCONNECTED;
    	emit stateChanged(DISCONNECTED);
        emit stateChanged("disconnected");        
    }
}
void OutputIceCast::disconnectStream()
{
    shout_close(_shout);
    _state = DISCONNECTED;
    emit stateChanged(DISCONNECTED);
    emit stateChanged("disconnected");
    emit message(QString("Disconnected from ") + _c.address);
}
void OutputIceCast::reconnectStream()
{
    _timer->start();
}
void OutputIceCast::output(const char* buffer, uint32_t size)
{
	if(getState() != CONNECTED)
		return;

	if(size != 0) {
        int r = shout_send(_shout, (const unsigned char*)buffer, size);        
		if(r != SHOUTERR_SUCCESS) {
			emit warn(QString("send error: ") + shout_get_error(_shout));            
			_state = DISCONNECTED;
			emit stateChanged(DISCONNECTED);
            emit stateChanged("disconnected");
            emit requestReconnect();
            _trial = 0;
		}
		shout_sync(_shout);
	}
}
QString OutputIceCast::getVersion() const
{
	QString s(shout_version(0,0,0));
	return s;
}
