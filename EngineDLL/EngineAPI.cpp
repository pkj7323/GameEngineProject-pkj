﻿#include "Common.h"
#include "CommonHeader.h"
#include "../Engine/Components/Script.h"
#include "../Graphics/Renderer.h"
#include "../Platform/PlatformTypes.h"
#include "../Platform/Platform.h"

#ifndef WIN32_MEAN_AND_LEAN
#define WIN32_MEAN_AND_LEAN
#endif


#include <OleCtl.h>
#include <Windows.h>


using namespace primal;

namespace 
{
	HMODULE game_code_dll{ nullptr };
	using _get_script_creator = primal::script::detail::script_creator(*)(size_t);
	_get_script_creator get_script_creator{ nullptr };

	using _get_script_names = LPSAFEARRAY(*)(void);
	_get_script_names get_script_names{ nullptr };

	utl::vector<graphics::render_surface> surfaces;
}

EDITOR_INTERFACE u32 LoadGameCodeDll(const char* dllpath)
{
	if (game_code_dll)
	{
		return FALSE;
	}
	game_code_dll = LoadLibraryA(dllpath);
	assert(game_code_dll);

	get_script_creator = (_get_script_creator)GetProcAddress(game_code_dll, "get_script_creator");
	get_script_names = (_get_script_names)GetProcAddress(game_code_dll, "get_script_names");

	return (game_code_dll && get_script_creator && get_script_names) ? TRUE : FALSE;
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

EDITOR_INTERFACE primal ::script::detail::script_creator GetScriptCreator(const char* name)
{
	return (game_code_dll && get_script_creator) ? get_script_creator(script::detail::string_hash{}(name)) : nullptr;
}

EDITOR_INTERFACE LPSAFEARRAY GetScriptNames()
{
	return (game_code_dll && get_script_names) ? get_script_names() : nullptr;
}

EDITOR_INTERFACE u32 CreateRenderSurface(HWND host, i32 width, i32 height) {

	platform::window_init_info info{ nullptr, host, nullptr , 0, 0, width, height };
	graphics::render_surface surface{ platform::create_window(&info),{} };
	assert(surface.window.is_valid());
	surfaces.emplace_back(surface);
	return static_cast<u32>(surfaces.size() - 1);
}

EDITOR_INTERFACE void RemoveRenderSurface(u32 id) {
	assert(id < surfaces.size());
	platform::remove_window(surfaces[id].window.getId());
	//surfaces.erase(surfaces.begin() + id);
}

EDITOR_INTERFACE HWND GetWindowHandle(u32 id) {
	assert(id < surfaces.size());
	return (HWND)surfaces[id].window.handle();
}

EDITOR_INTERFACE void ResizeRenderSurface(u32 id) {
	assert(id < surfaces.size());
	surfaces[id].window.resize(0, 0);
}