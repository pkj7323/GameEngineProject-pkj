#pragma once
#include "ComponentsCommon.h"


namespace primal
{
	//헤더 파일을 넣지않으면서 전방선언할때 줄수를 늘리고 싶지않으므로 전방선언용 매크로를 만들어준다.
#define INIT_INFO(component) namespace component { struct init_info; }
	INIT_INFO(transform);
#undef INIT_INFO

	namespace game_entity
	{
		struct entity_info
		{
			transform::init_info* transform{ nullptr };
		};

		entity create_game_entity(const entity_info& info);
		void remove_game_entity(entity e);
		bool is_alive(entity e);
	}
}
