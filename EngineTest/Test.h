﻿#pragma once
#include <thread>
#define TEST_ENTITY_COMPONENTS 0
#define TEST_WINDOW 0
#define TEST_RENDERER 1
class Test
{
public:
	virtual bool init() = 0;
	virtual void run() = 0;
	virtual void end() = 0;
};