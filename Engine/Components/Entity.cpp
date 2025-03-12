#include "Entity.h"
#include "Transform.h"
#include "Script.h"

namespace primal::game_entity
{
	/*namespace �̸����� ���ӽ����̽��� ���� Ŭ����ó�� ĸ��ȭ�� �� �� ����
	{
		u32 x;
		void somthing() {
			x = 0;
		}
	}*/

	namespace 
	{
		utl::vector<transform::component>	transforms;
		utl::vector<script::component>	scripts;

		utl::vector<id::generation_type>	generations;
		utl::deque<entity_id>				free_ids;
	}

	entity create(const entity_info& info)
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
			assert(!is_alive(id));
			free_ids.pop_front();
			id = entity_id{ id::new_generation(id) };
			++generations[id::index(id)];
		}
		else
		{
			id = entity_id{ (id::id_type)generations.size() };
			generations.push_back(0);

			// Resize components
			// Note: �츮�� resize�� ���ϱ� ������ ũ�⳶�� ���ϼ� ����
			transforms.emplace_back();
			scripts.emplace_back();
		}
		const entity new_entity{ id };
		const id::id_type index{ id::index(id) };

		//��ȯ ���۳�Ʈ ����ºκ�
		assert(!transforms[index].is_valid());
		transforms[index] = transform::create(*info.transform, new_entity);
		if (!transforms[index].is_valid())
		{
			return entity{};
		}
		//��ũ��Ʈ ����� �κ�
		if (info.script && info.script->script_creator)
		{
			assert(!scripts[index].is_valid());
			scripts[index] = script::create(*info.script, new_entity);
			assert(scripts[index].is_valid());
		}

		return new_entity;
	}

	void remove(entity_id id)
	{
		const id::id_type index{ id::index(id) };
		assert(is_alive(id));
		if (scripts[index].is_valid())
		{
			script::remove(scripts[index]);
			scripts[index] = {};
		}
		
		transform::remove(transforms[index]);
		transforms[index] = {};
		free_ids.push_back(id);
		
	}

	bool is_alive(entity_id id)
	{
		assert(id::is_valid(id));
		const id::id_type index{ id::index(id) };
		assert(index < generations.size());
		assert(generations[index] == id::generation(id));
		return (generations[index] == id::generation(id) && transforms[index].is_valid());
	}

	transform::component entity::transform() const
	{
		assert(is_alive(id_));
		const id::id_type index{ id::index(id_) };
		return transforms[index];
	}

	script::component entity::script() const
	{
		assert(is_alive(id_));
		const id::id_type index{ id::index(id_) };
		return scripts[index];
	}
}
