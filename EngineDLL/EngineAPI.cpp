﻿#include "Common.h"
#include "CommonHeader.h"
#ifndef WIN32_MEAN_AND_LEAN
#define WIN32_MEAN_AND_LEAN
#endif


#include <Windows.h>


using namespace primal;

namespace 
{
	HMODULE game_code_dll{ nullptr };
}

EDITOR_INTERFACE u32 LoadGameCodeDll(const char* dllpath)
{
	if (game_code_dll)
	{
		return FALSE;
	}
	game_code_dll = LoadLibraryA(dllpath);
	assert(game_code_dll);

	return game_code_dll ? TRUE : FALSE;
}
EDITOR_INTERFACE u32 UnloadGameCodeDll()
{
	if (!game_code_dll)
	{
		return FALSE;
	}
	assert(game_code_dll);
	int result{ FreeLibrary(game_code_dll) };
	assert(result);
	game_code_dll = nullptr;
	return TRUE;
}