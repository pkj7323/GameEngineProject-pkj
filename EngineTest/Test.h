#pragma once
#include <thread>
class Test
{
public:
	virtual bool init() = 0;
	virtual void run() = 0;
	virtual void end() = 0;
};