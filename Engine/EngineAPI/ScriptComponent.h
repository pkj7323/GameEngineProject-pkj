#pragma once
#include "../Components/ComponentsCommon.h"

namespace primal::script
{
	DEFINE_TYPED_ID(script_id);

	class component final
	{
	public:
		constexpr explicit component(script_id id) : id_{ id } {}
		constexpr component() : id_{ id::invalid_id } {}
		constexpr script_id getId() const { return id_; }
		constexpr bool is_valid() const { return id::is_valid(id_); }

	private:
		script_id id_;
	};


}
