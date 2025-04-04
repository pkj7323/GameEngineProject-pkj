#include "../Platform/PlatformTypes.h"
#include "../Platform/Platform.h"
#include "../Graphics/Renderer.h"
#include "TestRenderer.h"
#ifdef TEST_RENDERER

using namespace primal;
graphics::render_surface _surfaces[4];
void destroy_render_surface(graphics::render_surface& surface);
LRESULT win_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_DESTROY:
		{
			bool all_closed{ true };
			for (u32 i{ 0 }; i < _countof(_surfaces); ++i)
			{
				if (_surfaces[i].window.is_valid())
				{
					if (_surfaces[i].window.is_closed())
					{
						destroy_render_surface(_surfaces[i]);
					}
					else
					{
						all_closed = false;
						
					}
				}
				
			}
			if (all_closed)
			{
				PostQuitMessage(0);
				return 0;
			}
		}
		break;
		case WM_SYSCHAR:
			if (wParam == VK_RETURN && (HIWORD(lParam) & KF_ALTDOWN))
			{
				platform::window win{ platform::window_id{(id::id_type)GetWindowLongPtr(hWnd,GWLP_USERDATA)} };
				win.set_fullscreen(!win.is_fullscreen());
				return 0;
			}
			break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			PostMessage(hWnd, WM_CLOSE, 0, 0);
		}
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void create_render_surfaces(graphics::render_surface& surface, platform::window_init_info info)
{
	surface.window = platform::create_window(&info);
	surface.surface = graphics::create_surface(surface.window);
}

bool engine_test::init()
{
	bool result{ graphics::initialize(graphics::graphics_platform::direct3d12) };
	if (not result)
	{
		return result;
	}
	platform::window_init_info info[]
	{
		{&win_proc, nullptr,L"Render window 1",100,100,400,800},
		{&win_proc, nullptr,L"Render window 2",150,150,400,400},
		{&win_proc, nullptr,L"Render window 3",300,200,800,800},
		{&win_proc, nullptr,L"Render window 4",250,250,400,600},
	};
	static_assert(_countof(info) == _countof(_surfaces));
	for (u32 i{ 0 }; i < _countof(_surfaces); ++i)
	{
		create_render_surfaces(_surfaces[i], info[i]);
	}
	return true;
}
void engine_test::run()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	for (u32 i{ 0 }; i < _countof(_surfaces); ++i)
	{
		if (_surfaces[i].surface.is_valid())
		{
			_surfaces[i].surface.render();
		}
	}
	
}

void destroy_render_surface(graphics::render_surface& surface)
{
	graphics::render_surface temp{ surface };
	surface = {};
	if (temp.surface.is_valid())graphics::remove_surface(temp.surface.getId());
	if (temp.surface.is_valid())platform::remove_window(temp.window.getId());
	
}

void engine_test::end()
{
	for (u32 i{ 0 }; i < _countof(_surfaces); ++i)
		destroy_render_surface(_surfaces[i]);

	graphics::shutdown();
}
#endif