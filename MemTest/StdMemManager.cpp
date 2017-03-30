#include "stdafx.h"
#include "StdMemManager.h"


StdMemManager::StdMemManager()
{
}


StdMemManager::~StdMemManager()
{
}

void * StdMemManager::malloc(int len)
{
	return ::malloc(len);
}
void StdMemManager::free(void * p)
{
	::free(p);
}

