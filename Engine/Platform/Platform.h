#pragma once
#include "CommonHeader.h"
#include "Window.h"

namespace primal::platform
{
	struct window_init_info;
	window create_window(const window_init_info* init_info = nullptr);
	void remove_window(window_id id);
}