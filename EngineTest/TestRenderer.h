#pragma once
#include "Test.h"

class engine_test : public Test
{
public:
	engine_test() = default;
	~engine_test() = default;

	bool init() override;
	void run() override;
	void end() override;
};