#pragma once
class BaseMemManager
{
public:
	BaseMemManager();
	virtual ~BaseMemManager();

	virtual void * malloc(int len) = 0;
	virtual void free(void * p) = 0;
};

