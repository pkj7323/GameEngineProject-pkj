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

		//������ Ŭ���� �¾�

		//������ Ŭ���� ���

		//������ Ŭ���� �ν��Ͻ� ����
	}



#elif
#error "�ϳ� �̻��� �÷����� �����ؾ��մϴ�."
#endif
}
