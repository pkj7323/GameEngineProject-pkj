

#if !defined(SHIPPING)
#include "../Content/ContentLoader.h"
#include "../Components/Script.h"
#include "../Platform/Platform.h"
#include "../Platform/PlatformTypes.h"
#include "../Graphics/Renderer.h"
#include <thread>

using namespace primal;

namespace 
{
	graphics::render_surface game_window{};

	LRESULT win_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		switch (msg) {
		case WM_DESTROY:
		{
			
			if (game_window.window.is_closed()) {
				PostQuitMessage(0);
				return 0;
			}
		}
		break;
		case WM_SYSCHAR:
			if (wParam == VK_RETURN && (HIWORD(lParam) & KF_ALTDOWN)) {
				game_window.window.set_fullscreen(!game_window.window.is_fullscreen());
				return 0;
			}
			break;
		}
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}

bool engine_initialize()
{
	if (not primal::content::load_game())
		return false;

	platform::window_init_info info
	{
		&win_proc,nullptr, L"Primal Game",
	};

	game_window.window = platform::create_window(&info);
	if (not game_window.window.is_valid())
		return false;

	primal::script::begin_play();
	return true;
}
void engine_update()
{
	primal::script::update(10.f);
}
void engine_shutdown()
{
	platform::remove_window(game_window.window.getId());
	primal::content::unload_game();
}
#endif