#pragma once
#include "CommonHeader.h"
#include "Renderer.h"
#include "Platform/Window.h"

namespace primal::graphics
{
	struct platform_interface
	{
		bool(*initialize)(void);
		void(*shutdown)(void);
		

		struct 
		{
			std::function<surface(platform::window)> create;
			std::function<void(surface_id)> remove;
			std::function<void(surface_id, u32, u32)> resize;
			std::function<u32(surface_id)> width;
			std::function<u32(surface_id)> height;
			std::function<void(surface_id)> render;
		} surface;
		
	};
}
