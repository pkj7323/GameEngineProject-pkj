#include "Entity.h"
#include "Transform.h"
#include "../EngineAPI/GameEntity.h"

namespace primal::game_entity
{
	/*namespace 이름없는 네임스페이스를 통해 클래스처럼 캡슐화를 할 수 있음
	{
		u32 x;
		void somthing() {
			x = 0;
		}
	}*/

	namespace 
	{
		utl::vector<transform::component>	transforms;

		utl::vector<id::generation_type>	generations;
		utl::deque<entity_id>				free_ids;
	}

	entity create_game_entity(const entity_info& info)
	{
		assert(info.transform);
		if (!info.transform)
		{
			return {};
		}

		entity_id id;
		if (free_ids.size() > id::min_deleted_elements)
		{
			id = free_ids.front();
			assert(!is_alive(entity{ id }));
			free_ids.pop_front();
			id = entity_id{ id::new_generation(id) };
			++generations[id::index(id)];
		}
		else
		{
			id = entity_id{ static_cast<id::id_type>(generations.size()) };
			generations.push_back(0);

			// Resize components
			// Note: 이렇게 하면 미리 메모리를 할당해놓기 때문에 메모리를 할당하는데 드는 비용이 줄어든다.
			transforms.emplace_back();
		}
		const entity new_entity{ id };
		const id::id_type index{ id::index(id) };

		assert(!transforms[index].is_valid());
		transforms[index] = transform::create_transform(*info.transform, new_entity);
		if (!transforms[index].is_valid())
		{
			return entity{};
		}

		return new_entity;
	}

	void remove_game_entity(entity e)
	{
		const entity_id id{ e.getId() };
		const id::id_type index{ id::index(id) };
		assert(is_alive(e));
		if (is_alive(e))
		{
			transform::remove_transform(transforms[index]);
			transforms[index] = transform::component{};
			free_ids.push_back(id);
		}
	}

	bool is_alive(entity e)
	{
		assert(e.is_valid());
		const entity_id id{ e.getId() };
		const id::id_type index{ id::index(id) };
		assert(index < generations.size());
		assert(generations[index] == id::generation(id));
		return (generations[index] == id::generation(id) && transforms[index].is_valid());
	}

	transform::component entity::transform() const
	{
		assert(is_alive(*this));
		const id::id_type index{ id::index(id_) };
		return transforms[index];
	}
}
