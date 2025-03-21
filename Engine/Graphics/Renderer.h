#pragma once
#include "CommonHeader.h"
#include "../Platform/Window.h"

namespace primal::graphics
{
	class surface {
		
	};
	struct render_surface {
		platform::window window{};
		surface surface{};
	};
}
