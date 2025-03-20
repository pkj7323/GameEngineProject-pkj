#include "Platform.h"
#include "PlatformTypes.h"
namespace primal::platform
{
#ifdef _WIN64
	namespace 
	{
		
	}

	window create_window(const window_init_info* init_info /*= nullptr*/) {
		window_proc callback{ init_info ? init_info->callback : nullptr };
		window_handle parent{ init_info ? init_info->parent : nullptr };

		//윈도우 클래스 셋업

		//윈도우 클래스 등록

		//윈도우 클래스 인스턴스 생성
	}



#elif
#error "하나 이상의 플랫폼을 선택해야합니다."
#endif
}
