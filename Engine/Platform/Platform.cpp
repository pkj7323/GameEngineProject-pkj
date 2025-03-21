#include "Platform.h"
#include "PlatformTypes.h"
namespace primal::platform
{
#ifdef _WIN64
	namespace 
	{
		struct window_info {
			HWND	hWnd{ nullptr };
			RECT	client_area{ 0,0,1920,1080 };
			RECT	fullscreen_area{};
			POINT	top_left{ 0,0 };
			DWORD	style{ WS_VISIBLE };
			bool	is_fullscreen{ false };
			bool	is_closed{ false };
		};

		utl::vector<window_info> windows;
		//-------------------------------------------------------------------------
		// TODO: 이부분은 나중에 free-list 컨테이너로 변경해야함
		utl::vector<u32> available_slots;
		u32 add_to_window(const window_info& info) {
			u32 id{ u32_invalid_id };
			if (available_slots.empty())
			{
				id = static_cast<u32>(windows.size());
				windows.emplace_back(info);
			}
			else
			{
				id = available_slots.back();
				available_slots.pop_back();
				assert(id != u32_invalid_id);
				windows[id] = info;
			}
			return id;
		}
		void remove_from_window(u32 id) {
			assert(id < windows.size());
			available_slots.emplace_back(id);
		}
		//-------------------------------------------------------------------------
		window_info& get_from_id(window_id id) {
			assert(id < windows.size());
			assert(windows[id].hWnd);
			return windows[id];
		}
		window_info& get_from_handle(HWND hWnd) {
			const window_id id{ static_cast<id::id_type>(GetWindowLongPtr(hWnd, GWLP_USERDATA)) };
			return get_from_id(id);
		}

		LRESULT CALLBACK internal_window_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
			window_info* info{ nullptr };
			switch (msg)
			{
			case WM_DESTROY:
				get_from_handle(hWnd).is_closed = true;
				break;
			case WM_EXITSIZEMOVE:
				info = &get_from_handle(hWnd);
				break;
			case WM_SIZE:
				if (wParam == SIZE_MAXIMIZED) {
					info = &get_from_handle(hWnd);
				}
				break;
			case WM_SYSCOMMAND:
				if (wParam == SC_RESTORE) {
					info = &get_from_handle(hWnd);
				}
				break;
			default:
				break;
			}
			if (info)
			{
				assert(info->hWnd);
				GetClientRect(info->hWnd, info->is_fullscreen ? &info->fullscreen_area : &info->client_area);
			}

			LONG_PTR long_ptr{ GetWindowLongPtr(hWnd, 0) };
			return long_ptr
				       ? reinterpret_cast<window_proc>(long_ptr)(hWnd, msg, wParam, lParam)
				       : DefWindowProc(hWnd, msg, wParam, lParam);
		}

		void resize_window(const window_info& info, const RECT& area) {
			//윈도우 사이즈를 현재 기기에 맞게 조정
			RECT window_rect{ area };
			AdjustWindowRect(&window_rect, info.style, FALSE);

			const i32 width{ window_rect.right - window_rect.left };
			const i32 height{ window_rect.bottom - window_rect.top };

			MoveWindow(info.hWnd, info.top_left.x, info.top_left.y, width, height, true);
		}

		void resize_window(window_id id, u32 width, u32 height) {
			window_info& info{ get_from_id(id) };

			//유저가 스크린 해상도를 바꿧을때 풀스크린 과 창모드 둘다 리사이즈해야함 
			RECT& area{ info.is_fullscreen ? info.fullscreen_area : info.client_area };
			area.bottom = area.top + height;
			area.right = area.left + width;
			resize_window(info, area);
		}


		void set_window_fullscreen(window_id id, bool is_fullscreen) {
			window_info& info{ get_from_id(id) };
			if (info.is_fullscreen != is_fullscreen) {

				info.is_fullscreen = is_fullscreen;
				if (is_fullscreen) {
					GetClientRect(info.hWnd, &info.client_area);
					RECT rect;
					GetWindowRect(info.hWnd, &rect);
					info.top_left.x = rect.left;
					info.top_left.y = rect.top;
					info.style = 0;
					SetWindowLongPtr(info.hWnd, GWL_STYLE, info.style);
					ShowWindow(info.hWnd, SW_MAXIMIZE);
				}
				else {
					info.style = WS_VISIBLE | WS_OVERLAPPEDWINDOW;
					SetWindowLongPtr(info.hWnd, GWL_STYLE, info.style);
					resize_window(info, info.client_area);
					ShowWindow(info.hWnd, SW_SHOWNORMAL);
				}
				
			}
		}

		bool is_window_fullscreen(window_id id) {
			return get_from_id(id).is_fullscreen;
		}

		window_handle get_window_handle(window_id id) {
			return get_from_id(id).hWnd;
		}
		void set_window_caption(window_id id, const wchar_t* caption) {
			window_info& info{ get_from_id(id) };
			SetWindowText(info.hWnd, caption);
		}
		math::u32v4 get_window_size(window_id id) {
			window_info& info{ get_from_id(id) };
			RECT rect{ info.is_fullscreen ? info.fullscreen_area : info.client_area };
			return { (u32)rect.left,(u32)rect.top,(u32)rect.right,(u32)rect.bottom};
		}
		bool is_window_closed(window_id id) {
			return get_from_id(id).is_closed;
		}
		
	}





	window create_window(const window_init_info* init_info /*= nullptr*/) {
		window_proc callback{ init_info ? init_info->callback : nullptr };
		window_handle parent{ init_info ? init_info->parent : nullptr };

		//윈도우 클래스 셋업
		WNDCLASSEX wc;
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = internal_window_proc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = callback ? sizeof(window_proc) : 0;
		wc.hInstance = 0;
		wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wc.hbrBackground = CreateSolidBrush(RGB(26, 48, 76));
		wc.lpszMenuName = nullptr;
		wc.lpszClassName = L"primal_window";
		wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

		//윈도우 클래스 등록
		RegisterClassEx(&wc);

		window_info info{};
		RECT rc{ info.client_area };
		//윈도우 크기 조정
		AdjustWindowRect(&rc, info.style, FALSE);


		const wchar_t* caption{ (init_info && init_info->caption) ? init_info->caption : L"Primal Game" };
		const i32 left{ (init_info && init_info->left) ? init_info->left : info.client_area.left };
		const i32 top{ (init_info && init_info->top) ? init_info->top : info.client_area.top };
		const i32 width{ (init_info && init_info->width) ? init_info->width : rc.right - rc.left };
		const i32 height{ (init_info && init_info->height) ? init_info->height : rc.bottom - rc.top };


		info.style |= parent ? WS_CHILD : WS_OVERLAPPEDWINDOW;
		//윈도우 클래스 인스턴스 생성
		info.hWnd = CreateWindowEx(
			0,
			wc.lpszClassName,
			caption,
			info.style,
			left,
			top,
			width,
			height,
			parent,
			nullptr, 
			nullptr,
			nullptr);

		if (info.hWnd)
		{
			SetLastError(0);

			const window_id id{ add_to_window(info) };
			SetWindowLongPtr(info.hWnd, GWLP_USERDATA, (LONG_PTR)id);
			// 윈도우 메세지를 핸들링하는 윈도우 콜백 함수 포인터를 위해
			// "여분"의 메모리를 할당한다.
			if (callback) 
				SetWindowLongPtr(info.hWnd, 0, reinterpret_cast<LONG_PTR>(callback));
			assert(GetLastError() == 0);
			ShowWindow(info.hWnd, SW_SHOWNORMAL);
			UpdateWindow(info.hWnd);
			return window{ id };
		}


		return {};
	}

	void remove_window(window_id id) {
		window_info& info{ get_from_id(id) };
		DestroyWindow(info.hWnd);
		remove_from_window(id);
	}

#elif
#error "하나 이상의 플랫폼을 선택해야합니다."
#endif // _WIN64
	

	void window::set_fullscreen(bool is_fullscreen) const {
		assert(is_valid());
		set_window_fullscreen(id_, is_fullscreen);
	}

	

	bool window::is_fullscreen() const {
		assert(is_valid());
		return is_window_fullscreen(id_);
	}


	void* window::handle() const {
		assert(is_valid());
		return get_window_handle(id_);
	}

	

	void window::set_caption(const wchar_t* caption) const {
		assert(is_valid());
		set_window_caption(id_, caption);
	}
	const math::u32v4 window::size() const {
		assert(is_valid());
		return get_window_size(id_);
	}
	void window::resize(u32 width, u32 height) const {
		assert(is_valid());
		resize_window(id_, width, height);
	}
	const u32 window::get_width() const {
		math::u32v4 s{ size() };
		return s.z - s.x;
	}
	const u32 window::get_height() const {
		math::u32v4 s{ size() };
		return s.w - s.y;
	}
	bool window::is_closed() const {
		assert(is_valid());
		return is_window_closed(id_);
	}
}
