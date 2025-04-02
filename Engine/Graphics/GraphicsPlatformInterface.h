#pragma once
#include "CommonHeader.h"
#include "Renderer.h"

namespace primal::graphics
{
	struct platform_interface
	{
		std::function<bool(void)> initialize;
		std::function<void(void)> shutdown;
		std::function<void(void)> render;

		
	};
}
