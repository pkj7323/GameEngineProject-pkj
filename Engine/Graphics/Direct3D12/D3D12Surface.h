#pragma once
#include "D3D12CommonHeader.h"
#include "D3D12Resources.h"

namespace primal::graphics::d3d12
{
	class d3d12_surface
	{
	public:
		explicit d3d12_surface(platform::window window) : window_{ window }
		{
			assert(window_.handle());
		}
		~d3d12_surface()
		{
			release();
		}

		void create_swap_chain(IDXGIFactory7* factory, ID3D12CommandQueue* command_queue, DXGI_FORMAT format);
		void present() const;
		void resize();

		u32 width() const;
		u32 height() const;

	private:
		void release();
		void finalize();

		struct render_target_data
		{
			ID3D12Resource* resource{ nullptr };
			descriptor_handle rtv{};
		};

		IDXGISwapChain4*	swap_chain_{ nullptr };
		render_target_data	render_target_data_[frame_buffer_count]{};
		platform::window	window_{};
		u32					current_bb_index{0};
	};
	
}
