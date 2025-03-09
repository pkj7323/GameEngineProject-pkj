#pragma once
#include <random>

#include "CommonHeader.h"


namespace primal::id
{
	using id_type = u32;//원하는 unsigned integer type을 사용할 수 있다.

	namespace internal
	{
		constexpr u32 generation_bits{ 10 };
		constexpr u32 index_bits{ sizeof(id_type) * 8 - generation_bits };
		constexpr id_type index_mask{ (id_type{1} << index_bits) - 1 }; //0111
		constexpr id_type generation_mask{ (id_type{1} << generation_bits) - 1 }; //1000
	}

	constexpr id_type invalid_id{ static_cast<id_type>(-1)};
	constexpr u32 min_deleted_elements{ 1024 };
	//template<bool condition, class t1, class t2>
	//struct conditional
	//{
	//	using type = t1; //컨디션이 참일 때는 1번째 타입을 반환
	//};
	//template<class t1, class t2>
	//struct conditional<false, t1, t2>
	//{
	//	using type = t2; //컨디션이 거짓일 때는 2번째 타입을 반환
	//};

	//generation_bits <= 8이면 generation_type은 u8입니다.
	//generation_bits > 8이고 generation_bits <= 16이면 generation_type은 u16입니다.
	//generation_bits > 16이면 generation_type은 u32입니다.
	using generation_type = std::conditional_t<internal::generation_bits <= 16, std::conditional_t<internal::generation_bits <= 8, u8, u16>, u32>;
	static_assert(sizeof(generation_type) * 8 >= internal::generation_bits);
	static_assert((sizeof(id_type) - sizeof(generation_type)) > 0);

	constexpr bool is_valid(id_type id)
	{
		return id != invalid_id;
	}
	constexpr id_type index(id_type id)
	{
		id_type index{ id & internal::index_mask };
		assert(index != internal::index_mask);
		return id & internal::index_mask;
	}
	constexpr id_type generation(id_type id)
	{
		return (id >> internal::index_bits) & internal::generation_mask;
	}
	constexpr id_type new_generation(id_type id)
	{
		const id_type generation{ id::generation(id) + 1 };
		assert(generation < ((u64)1 << internal::generation_bits) - 1);
		return index(id) | (generation << internal::index_bits);
		
	}

#if _DEBUG
	namespace internal
	{
		struct id_base
		{
			constexpr explicit id_base(id_type id) : id{ id } {}
			constexpr operator id_type() const { return id; }
		private:
			id_type id;
		};
	}

#define DEFINE_TYPED_ID(name)															\
			struct name final : id::internal::id_base									\
			{																			\
				constexpr explicit name(id::id_type id) : id_base{ id } {}				\
				constexpr name() : id_base{ 0 } {}				\
			};
#else
#define DEFINE_TYPED_ID(name) using name = id::id_type;
#endif

}
