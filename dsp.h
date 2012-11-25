#ifndef DSP_H
#define DSP_H

#include <QThread>

class DSP : public QThread
{
public:
    DSP();

    void run();
    bool isActive() const { return _active; }
    void disable() { _active = false; }
private:
	bool _active;

};

#endif // DSP_H
