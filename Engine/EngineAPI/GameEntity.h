#pragma once

#include "../Components/ComponentsCommon.h"
#include "TranformComponent.h"
#include "ScriptComponent.h"

namespace primal
{
	namespace game_entity
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
			script::component script() const;
		private:
			entity_id id_{ id::invalid_id };
		};
	}

	namespace script
	{
		class entity_script : public game_entity::entity
		{
		public:
			virtual ~entity_script() = default;
			virtual void begin_play() {}
			virtual void update(float) {}
		protected:
			constexpr explicit entity_script(game_entity::entity entity)
				: game_entity::entity{ entity.getId() }
			{

			}
		};

		namespace detail
		{
			using script_ptr = std::unique_ptr<entity_script>;
			using script_creator = script_ptr(*)(game_entity::entity entity);
			using string_hash = std::hash<std::string>;
			u8 register_script(size_t, script_creator);

			template<class script_class>
			script_ptr create_script(game_entity::entity entity)
			{
				assert(entity.is_valid());
				return std::make_unique<script_class>(entity);
			}
		}
		
#define REGISTER_SCRIPT(TYPE)									\
		class TYPE;												\
		namespace												\
		{														\
			const u8 _reg##TYPE{								\
				primal::script::detail::register_script(		\
				primal::script::detail::string_hash()(#TYPE),	\
				&primal::script::detail::create_script<TYPE>)	\
			};													\
		}


	}
}
