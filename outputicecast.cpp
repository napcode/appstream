#include "outputicecast.h"
#include "logger.h"

#include <QSettings>

OutputIceCast::OutputIceCast()
:	_shout(0),
	_state(INVALID)
{
    shout_init();
}
OutputIceCast::~OutputIceCast()
{
	shout_close(_shout);
	shout_free(_shout);
	shout_shutdown();
}
bool OutputIceCast::init()
{
	//shout_init();
	_shout = shout_new();
	if(!_shout)
		return false;

	QSettings s;
	s.beginGroup("connection");
	if(!s.contains("selected")) {
        emit message("Error: no connection selected.");
		return false;
	}
	s.beginGroup(s.value("selected").toString());
	if( !s.contains("address") ||
		!s.contains("password") ||
		!s.contains("port") ||
		!s.contains("mountpoint") ) {
		emit message("Error: connection configuration invalid");
		return false;
	}

    _c.address = s.value("address").toString();
    _c.port = s.value("port").toInt();
    _c.user = QString("appStream");
    _c.password = s.value("password").toString();
    _c.mountpoint = s.value("mountpoint").toString();
    // std::cout << "addr " << _c.address.toStdString() << std::endl;
    // std::cout << "port " << _c.port << std::endl;
    // std::cout << "user " << _c.user.toStdString() << std::endl;
    // std::cout << "password " << _c.password.toStdString() << std::endl;
    // std::cout << "mountpoint " << _c.mountpoint.toStdString() << std::endl;
	int r;
    r = shout_set_host(_shout, "_c.address.toStdString().c_str()");
    if(r != SHOUTERR_SUCCESS) {
        emit error(QString("unable to set hostname: ") + shout_get_error(_shout));
    	return false;    	
    }
	 std::cout << "atest" << std::endl;
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
    	r = shout_set_protocol(_shout, SHOUT_FORMAT_MP3);
    else if(_c.encoder == QString("Ogg Vorbis"))
    	r = shout_set_protocol(_shout, SHOUT_FORMAT_OGG);
    if(r != SHOUTERR_SUCCESS) {
        emit error(QString("unable to set protocol: ") + shout_get_error(_shout));
    	return false;    	
    }

    setStreamInfo();
    _state = READY;
    emit stateChanged(READY);
	return true;
}
void OutputIceCast::setStreamInfo()
{
	int r;
    r = shout_set_name(_shout, "teststream");
    if(r != SHOUTERR_SUCCESS) {
        emit error(QString("unable to set name: ") + shout_get_error(_shout)); 	
    }
}
void OutputIceCast::connect()
{
	if(getState() != READY)
		return;

    int r = shout_open(_shout);
    if(r == SHOUTERR_SUCCESS) {
        emit message(QString("Connected to ") + _c.address);
    	_state = CONNECTED;
    	emit stateChanged(CONNECTED);
    }
    else {
    	_state = DISCONNECTED;
    	emit stateChanged(DISCONNECTED);	
    }
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
		}
		shout_sync(_shout);
	} 	
}
QString OutputIceCast::getVersion() const
{
	QString s(shout_version(0,0,0));
	return s;
}