#pragma once
#include "CommonHeader.h"

#ifdef _WIN64
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

namespace primal::platform
{
	using window_proc = LRESULT(*)(HWND, UINT, WPARAM, LPARAM);
	using window_handle = HWND;

	struct window_init_info {
		window_proc		callback{ nullptr };
		window_handle	parent{ nullptr };
		const char*		caption{ nullptr };
		i32				left{ 0 };
		i32				top{ 0 };
		i32				width{ 1920 };
		i32				height{ 1080 };

	};
}
#endif