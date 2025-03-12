#pragma once
#include "ComponentsCommon.h"


namespace primal
{
	//헤더 파일을 넣지않으면서 전방선언할때 줄수를 늘리고 싶지않으므로 전방선언용 매크로를 만들어준다.
#define INIT_INFO(component) namespace component { struct init_info; }
	INIT_INFO(transform);
	INIT_INFO(script);
#undef INIT_INFO

	namespace game_entity
	{
		struct entity_info
		{
			transform::init_info* transform{ nullptr };
			script::init_info* script{ nullptr };
		};

		entity create(const entity_info& info);
		void remove(entity_id e);
		bool is_alive(entity_id e);
	}
	
}
