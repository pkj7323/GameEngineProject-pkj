#include "Renderer.h"
#include "GraphicsPlatformInterface.h"

namespace primal::graphics
{
	namespace 
	{
		platform_interface gfx{};

		bool set_platform_interface(graphics_platform platform)
		{
			switch (platform)
			{
				case graphics_platform::direct3d12:
					d3d12::get_platform_interface(gfx);
					
					break;
				case graphics_platform::vulkan:
					assert("아직 vulkan api없음");
					break;
				case graphics_platform::open_gl:
					assert("아직 opengl api없음");
					
					break;
				default:
					return false;
			}
			return true;
		}

		bool initialize(graphics_platform platform)
		{
			return set_platform_interface(platform);
		}

		void shutdown()
		{
			gfx.shutdown();
		}
	}
}
