#pragma once


#include "Test.h"
#include "../Platform/Platform.h"
#include "../Platform/PlatformTypes.h"

using namespace primal;

primal::platform::window _windows[4];

LRESULT win_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_DESTROY:
	{
		bool all_closed{ true };
		for (u32 i{ 0 }; i < _countof(_windows); ++i) {
			if (!_windows[i].is_closed())
			{
				all_closed = false;
			
			}
		}
		if (all_closed)
		{
			PostQuitMessage(0);
			return 0 ;
		}
	}
		break;
	case WM_SYSCHAR:
		if (wParam == VK_RETURN && (HIWORD(lParam)&KF_ALTDOWN))
		{
			platform::window win{ platform::window_id{(id::id_type)GetWindowLongPtr(hWnd,GWLP_USERDATA)} };
			win.set_fullscreen(!win.is_fullscreen());
			return 0;
		}
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}


class engine_test : public Test {
public:
	bool init() override {
		platform::window_init_info info[]
		{
			{&win_proc, nullptr,L"Test window 1",100,100,400,800},
			{&win_proc, nullptr,L"Test window 2",150,150,400,400},
			{&win_proc, nullptr,L"Test window 3",300,200,800,800},
			{&win_proc, nullptr,L"Test window 4",250,250,400,600},
		};
		static_assert(_countof(info) == _countof(_windows));
		for (u32 i{ 0 }; i < 4; ++i) {
			_windows[i] = primal::platform::create_window(&info[i]);
		}
		return true;
	}
	void run() override {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	void end() override {
		for (u32 i = 0; i < _countof(_windows); ++i) {
			platform::remove_window(_windows[i].getId());
		}
	}
};
