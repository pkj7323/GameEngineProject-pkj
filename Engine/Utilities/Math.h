#pragma once
#include "CommonHeader.h"
#include "MathTypes.h"

namespace primal::math
{
	template<typename T >
	constexpr T clamp(const T& value, const T& min, const T& max) {
		return std::clamp(value, min, max);
	}
	template <class T, class _Pr>
	constexpr T clamp(const T& value, const T& min, const T& max, _Pr pr) {
		return std::clamp(value, min, max, pr);
	}


	template<u32 bits>
	constexpr u32 pack_unit_float(f32 f)
	{
		static_assert(bits <= sizeof(u32) * 8);
		assert(f >= 0.f && f <= 1.f);
		constexpr f32 intervals{ static_cast<f32>((1ui32 << bits) - 1) };
		return static_cast<u32>(intervals * f + 0.5f);
	}

	template<u32 bits>
	constexpr f32 unpack_to_unit_float(u32 i) {
		static_assert(bits <= sizeof(u32) * 8);
		assert(i < (1ui32 << bits));
		constexpr f32 intervals{ static_cast<f32>((1ui32 << bits) - 1) };
		return static_cast<f32>(i) / intervals;
	}

	template<u32 bits>
	constexpr u32 pack_float(f32 f, f32 min, f32 max) {
		
		assert(min < max);
		assert(f >= min && f <= max);
		const f32 distance{ (f - min) / (max - min) };
		return pack_unit_float<bits>(distance);
	}

	template<u32 bits>
	constexpr f32 unpack_to_float(u32 i, f32 min, f32 max) {

		assert(min < max);
		return unpack_to_unit_float<bits>((i) * (max - min) + min);
	}



}
