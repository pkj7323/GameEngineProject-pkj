#include "Script.h"
#include "Entity.h"
#include <atlsafe.h>

namespace primal::script
{
	namespace 
	{
		utl::vector<detail::script_ptr>		entity_scripts;
		utl::vector<id::id_type>			id_mapping;

		utl::vector<id::generation_type>	generaions;
		utl::vector<script_id>				free_ids;

		using script_registry = std::unordered_map<size_t, detail::script_creator>;
		

		script_registry& registry()
		{
			/// NOTE: 우리는 정적 초기화 명령 때문에 함수안에 이 정적 변수 를 넣는다. 
			///			이때, 우리는 변수에 접근하기전에 데이터가 초기화 되어있는지 확실히 할 수 있다.
			static script_registry reg;
			return reg;
		}
#ifdef USE_WITH_EDITOR
		utl::vector<std::string>& script_names()
		{
			static utl::vector<std::string> names;
			return names;
		}
#endif




		bool exists(script_id id)
		{
			assert(id::is_valid(id));
			const id::id_type index{ id::index(id) };
			assert(index < generaions.size() && id_mapping[index] < entity_scripts.size());
			assert(generaions[index] == id::generation(id));
			return (generaions[index] == id::generation(id)) &&
				entity_scripts[id_mapping[index]] &&
				entity_scripts[id_mapping[index]]->is_valid();
		}
	}

	

	

	namespace detail
	{
		u8 register_script(size_t tag, script_creator func)
		{
			bool result{ registry().insert(script_registry::value_type{tag, func}).second };
			assert(result);
			return result;
		}

		script_creator get_script_creator(size_t tag)
		{
			auto script = primal::script::registry().find(tag);
			assert(script != primal::script::registry().end() && script->first == tag);
			return script->second;
		}

#ifdef USE_WITH_EDITOR
		u8 add_script_name(const char* name)
		{
			script_names().emplace_back(name);
			return true;
		}
#endif
	}
	component create(init_info info, game_entity::entity entity)
	{
		assert(entity.is_valid());
		assert(info.script_creator);

		script_id id{};
		if (free_ids.size() > id::min_deleted_elements)
		{
			id = free_ids.front();
			assert(!exists(id));
			free_ids.pop_back();
			id = script_id{ id::new_generation(id) };
			++generaions[id::index(id)];
		}
		else
		{
			id = script_id{ static_cast<id::id_type>(id_mapping.size()) };
			id_mapping.emplace_back();
			generaions.push_back(0);
		}

		assert(id::is_valid(id));
		const id::id_type index{ (id::id_type)entity_scripts.size() };
		entity_scripts.emplace_back(info.script_creator(entity));
		assert(entity_scripts.back()->getId() == entity.getId());
		id_mapping[id::index(id)] = index;
		return component{ id };
	}
	void remove(component c)
	{
		assert(c.is_valid() && exists(c.getId()));
		const script_id id{ c.getId() };
		const id::id_type index{ id_mapping[id::index(id)] };
		const script_id last_id{ entity_scripts.back()->script().getId() };
		utl::erase_unordered(entity_scripts, index);
		id_mapping[id::index(last_id)] = index;
		id_mapping[id::index(id)] = id::invalid_id;
	}
	void update(float dt)
	{
		for (auto& ptr : entity_scripts)
		{
			ptr->update(dt);
		}
	}
	void begin_play() {
		for (auto& ptr : entity_scripts) {
			ptr->begin_play();
		}
	}

#ifdef USE_WITH_EDITOR


	extern "C" __declspec(dllexport) LPSAFEARRAY __stdcall get_script_names()
	{
		const u32 size{ (u32)primal::script::script_names().size() };
		if (!size) return nullptr;
		CComSafeArray<BSTR> names{ size };
		for (u32 i = 0; i < size; i++)
		{
			names.SetAt(i, A2BSTR_EX(primal::script::script_names()[i].c_str()), false);
		}
		return names.Detach();
	}
#endif // USE_WITH_EDITOR

}
