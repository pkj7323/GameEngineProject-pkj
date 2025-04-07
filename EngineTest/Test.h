#pragma once
#include <chrono>
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
#ifdef _WIN64
#include <Windows.h>
class time_it
{
public:
	using clock = std::chrono::high_resolution_clock;
	using time_stamp = std::chrono::steady_clock::time_point;

	void begin()
	{
		start_ = clock::now();
	}
	void end()
	{
		auto dt = clock::now() - start_;
		ms_avg_ += (std::chrono::duration<float, std::milli>(dt).count() - ms_avg_) / (float)counter_;
		++counter_;

		if (std::chrono::duration_cast<std::chrono::seconds>(clock::now() - seconds_).count() >= 1)
		{
			OutputDebugStringA("Avg frame (ms): ");
			OutputDebugStringA(std::to_string(ms_avg_).c_str());
			OutputDebugStringA((" " + std::to_string(counter_)).c_str());
			OutputDebugStringA(" fps");
			OutputDebugStringA("\n");
			ms_avg_ = 0.f;
			counter_ = 1;
			seconds_ = clock::now();
		}
	}
private:
	float ms_avg_{ 0.f };
	int counter_{ 1 };
	time_stamp start_{};
	time_stamp seconds_{ clock::now() };

};
#endif