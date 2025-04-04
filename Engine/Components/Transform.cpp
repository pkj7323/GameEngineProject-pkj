#include "Transform.h"
#include "Entity.h"

namespace primal::transform
{
	namespace 
	{
		utl::vector<math::v3> positions;
		utl::vector<math::v4> rotations;
		utl::vector<math::v3> scales;
	}

	component create(init_info info, game_entity::entity entity)
	{
		assert(entity.is_valid());
		const id::id_type entity_index{ id::index(entity.getId()) };

		if (positions.size() > entity_index)
		{
			rotations[entity_index] = math::v4{ info.rotation };
			positions[entity_index] = math::v3{ info.position };
			scales[entity_index] = math::v3{ info.scale };
		}
		else
		{
			assert(positions.size() == entity_index);
			positions.emplace_back(info.position);
			rotations.emplace_back(info.rotation);
			scales.emplace_back(info.scale);
		}
		return component(transform_id{ (id::id_type)positions.size() - 1});
	}

	void remove(component c)
	{
		assert(c.is_valid());
	}

	math::v3 component::position() const
	{
		assert(is_valid());
		return positions[id::index(id_)];
	}

	math::v4 component::rotation() const
	{
		assert(is_valid());
		return rotations[id::index(id_)];
	}

	math::v3 component::scale() const
	{
		assert(is_valid());
		return scales[id::index(id_)];
	}
}
