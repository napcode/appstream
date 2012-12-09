#ifndef OUTPUTICECAST_H
#define OUTPUTICECAST_H

#include "output.h"

#include <shout/shout.h>

struct ConfigIceCast
{
	QString address;
	QString user;
	QString password;
	QString mountpoint;
	QString encoder;
	uint16_t port;
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

	void setStreamInfo();
	State getState() const { return _state; }
	QString getVersion() const;
signals:
	void stateChanged(State);
private:
	shout_t *_shout;
	State _state;
    ConfigIceCast _c;
};

#endif
