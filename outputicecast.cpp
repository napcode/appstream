#include "outputicecast.h"

#include <shout/shout.h>

OutputIceCast::OutputIceCast()
{

}
OutputIceCast::~OutputIceCast()
{
	shout_shutdown();
}
bool OutputIceCast::init()
{
	shout_init();
	return true;
}
