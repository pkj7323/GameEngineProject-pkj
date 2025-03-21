
#pragma comment(lib, "Engine.lib")

#define TEST_ENTITY_COMPONENTS 0
#define TEST_WINDOW 1

#if TEST_ENTITY_COMPONENTS
#include "TestEntityComponents.h"
#elif TEST_WINDOW
#include "TestWindow.h"
#else
#error 하나 이상의 테스트를 선택해야합니다.
#endif

#ifdef _WIN64
#include <Windows.h>
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#if _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	engine_test test{};
	if (test.init()) {
		MSG msg;
		bool is_running{ true };
		while (is_running) {
			while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);

				is_running &= (msg.message != WM_QUIT);
			}

			test.run();
		}
	}
	test.end();
	return 0;
}
#else
int main() {
#if _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	engine_test test{ };

	if (test.init()) {
		test.run();
	}
	test.end();
	return 0;
}
#endif

