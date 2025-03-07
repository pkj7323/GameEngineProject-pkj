#pragma once
#include <stdint.h>

//unsigend int 이름 길어서 변경

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
//모든 비트에서 최댓값비트를 invaild bit로 사용 할것임

//int 이름 길어서 변경
using s8 = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;

constexpr u8 u8_invalid_id{ 0xffui64 };
constexpr u16 u16_invalid_id{ 0xffffui64 };
constexpr u32 u32_invalid_id{ 0xffff'ffffui64 };
constexpr u64 u64_invalid_id{ 0xffff'ffff'ffff'ffffui64 };

//float 이름 길어서 변경
using f32 = float;