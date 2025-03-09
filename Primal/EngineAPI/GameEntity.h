#pragma once

#include "../Components/ComponentsCommon.h"
#include "TranformComponent.h"

namespace primal::game_entity
{
	DEFINE_TYPED_ID(entity_id);
	class entity
	{
	public:
		constexpr explicit entity(entity_id id): id_{ id } {}
		constexpr entity(): id_ { id::invalid_id } {}
		constexpr entity_id getId() const { return id_; }
		constexpr bool is_valid() const { return id::is_valid(id_); }

		transform::component transform() const;
	private:
		entity_id id_{ id::invalid_id };
	};
}
