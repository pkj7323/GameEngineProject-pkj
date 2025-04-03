#pragma once
#include "D3D12CommonHeader.h"
#include "D3D12Resources.h"

namespace primal::graphics::d3d12::core
{
	bool initialize();
	void shutdown();
	void render();
	template<typename T >
	constexpr void release(T*& resource)
	{
		if (resource)
		{
			resource->Release();
			resource = nullptr;
		}
	}

	namespace detail
	{
		void deferred_release(IUnknown* resource);
	}

	template<typename T >
	constexpr void defered_release(T*& resource)
	{
		if (resource)
		{
			detail::deferred_release(resource);
			resource = nullptr;
		}
	}
	ID3D12Device* const device();
	descriptor_heap& rtv_heap();
	descriptor_heap& dsv_heap();
	descriptor_heap& srv_heap();
	descriptor_heap& uav_heap();
	u32 current_frame_index();
	void set_deferred_release_flag();
}
