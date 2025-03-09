
#pragma comment(lib, "Engine.lib")

#define TEST_ENTITY_COMPONENTS 1

#if TEST_ENTITY_COMPONENTS
#include "TestEntityComponents.h"
#else
#error 하나 이상의 테스트를 선택해야합니다.
#endif


int main()
{
#if _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	engine_test test{ };

	if (test.init())
	{
		test.run();
	}
	test.end();
	return 0;
}
