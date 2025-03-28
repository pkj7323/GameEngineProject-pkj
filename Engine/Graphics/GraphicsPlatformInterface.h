#pragma once
#include "CommonHeader.h"
#include "Renderer.h"

namespace primal::graphics
{
	struct platform_interface
	{
		bool (*initialize)(void);
		void (*shutdown)(void);
	};
}
