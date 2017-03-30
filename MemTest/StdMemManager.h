#pragma once
#include "BaseMemManager.h"
class StdMemManager :
	public BaseMemManager
{
public:
	StdMemManager();
	virtual ~StdMemManager();

	virtual void * malloc(int len);
	virtual void free(void * p);

};


