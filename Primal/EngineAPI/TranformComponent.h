#pragma once
#include "../Components/ComponentsCommon.h"

namespace primal::transform
{
	DEFINE_TYPED_ID(transform_id);

	class component final
	{
	public:
		constexpr explicit component(transform_id id) : id_{ id } {}
		constexpr component() : id_{ id::invalid_id } {}
		constexpr transform_id getId() const { return id_; }
		constexpr bool is_valid() const { return id::is_valid(id_); }

		math::v3 position() const;
		math::v4 rotation() const;
		math::v3 scale() const;
	private:
		transform_id id_;
	};

	
}
