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
#if USE_STL_VECTOR
		DISABLE_COPY(d3d12_surface);
		constexpr d3d12_surface(d3d12_surface&& other) : swap_chain_{ other.swap_chain_ },
			window_{ other.window_ },
			current_bb_index{ other.current_bb_index },
			viewport_{ other.viewport_ },
			scissor_rect_{ other.scissor_rect_ }
		{
			for (u32 i = 0; i < frame_buffer_count; ++i)
			{
				render_target_data_[i] = other.render_target_data_[i];
				render_target_data_[i].rtv = other.render_target_data_[i].rtv;
			}
			other.reset();
		}

		

		constexpr d3d12_surface& operator=(d3d12_surface&& other)
		{
			assert(this != &other);
			if (this != &other)
			{
				release();
				move(other);
			}
			return *this;
		}
#endif

		~d3d12_surface()
		{
			release();
		}

		void create_swap_chain(IDXGIFactory7* factory, ID3D12CommandQueue* command_queue, DXGI_FORMAT format);
		void present() const;
		void resize();

		constexpr u32 width() const { return (u32)viewport_.Width; }
		constexpr u32 height() const { return (u32)viewport_.Height; }
		constexpr ID3D12Resource* const back_buffer() const{return render_target_data_[current_bb_index].resource;}
		constexpr D3D12_CPU_DESCRIPTOR_HANDLE rtv() const{return render_target_data_[current_bb_index].rtv.cpu;}
		constexpr const D3D12_VIEWPORT& viewport() const { return viewport_; }
		constexpr const D3D12_RECT& scissor_rect() const { return scissor_rect_; }
	private:
		void release();
		void finalize();
#if USE_STL_VECTOR
		constexpr void reset()
		{
			swap_chain_ = nullptr;
			for (u32 i = 0; i < frame_buffer_count; ++i)
			{
				render_target_data_[i] = {};

			}
			window_ = {};
			current_bb_index = 0;
			viewport_ = {};
			scissor_rect_ = {};
		}
		constexpr void move(d3d12_surface& other)
		{
			swap_chain_ = other.swap_chain_;
			for (u32 i = 0; i < frame_buffer_count; ++i)
			{
				render_target_data_[i] = other.render_target_data_[i];
			}
			window_ = other.window_;
			current_bb_index = other.current_bb_index;
			viewport_ = other.viewport_;
			scissor_rect_ = other.scissor_rect_;

			other.reset();
		}
#endif;
		struct render_target_data
		{
			ID3D12Resource* resource{ nullptr };
			descriptor_handle rtv{};
		};

		IDXGISwapChain4*	swap_chain_{ nullptr };
		render_target_data	render_target_data_[frame_buffer_count]{};
		platform::window	window_{};
		mutable u32			current_bb_index{0};
		D3D12_VIEWPORT 		viewport_{};
		D3D12_RECT			scissor_rect_{};
	};
	
}
