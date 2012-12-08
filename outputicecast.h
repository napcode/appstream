#ifndef OUTPUTICECAST_H
#define OUTPUTICECAST_H

#include "output.h"

class OutputIceCast : public Output
{
public:
	OutputIceCast();
	~OutputIceCast();
	bool init();
	
};

#endif