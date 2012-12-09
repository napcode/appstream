#ifndef OUTPUTICECAST_H
#define OUTPUTICECAST_H

#include "output.h"

#include <shout/shout.h>

struct ConfigConnection
{
	QString address;
	QString user;
	QString password;
	QString mountpoint;
	QString encoder;
	int port;
};
struct ConfigStreamInfo
{
	QString name;
	QString description;
	QString agent;
	QString genre;
	QString url;
	bool isPublic;
};

class OutputIceCast : public Output
{
    Q_OBJECT
public:
	enum State 
	{
		INVALID,
		READY,
		CONNECTED,
		DISCONNECTED
	};

	OutputIceCast();
	~OutputIceCast();
	bool init();
    void output(const char* buffer, uint32_t size);
	void connect();

	void setConnection(const QString &name);
	void setConnection(const ConfigConnection &config);
	void setStreamInfo(const QString &name);
	void setStreamInfo(const ConfigStreamInfo &config);
	void setMetaData();
	
	State getState() const { return _state; }
	QString getVersion() const;
signals:
	void stateChanged(State);
private:
	void applyStreamInfo();

	shout_t *_shout;
	State _state;
    ConfigConnection _c;
    ConfigStreamInfo _csi;
};

#endif
