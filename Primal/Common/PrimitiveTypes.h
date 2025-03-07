#pragma once
#include <stdint.h>

//unsigend int �̸� �� ����

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
//��� ��Ʈ���� �ִ񰪺�Ʈ�� invaild bit�� ��� �Ұ���

//int �̸� �� ����
using s8 = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;

constexpr u8 u8_invalid_id{ 0xffui64 };
constexpr u16 u16_invalid_id{ 0xffffui64 };
constexpr u32 u32_invalid_id{ 0xffff'ffffui64 };
constexpr u64 u64_invalid_id{ 0xffff'ffff'ffff'ffffui64 };

//float �̸� �� ����
using f32 = float;