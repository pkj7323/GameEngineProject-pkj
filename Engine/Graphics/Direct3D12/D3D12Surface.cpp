#include "D3D12Surface.h"

#include "D3D12Core.h"

namespace primal::graphics::d3d12
{
	namespace 
	{
		constexpr DXGI_FORMAT to_non_srgb(DXGI_FORMAT format)
		{
			if (format == DXGI_FORMAT_B8G8R8A8_UNORM_SRGB)
			{
				return DXGI_FORMAT_B8G8R8A8_UNORM;
			}
			return format;
		}
		
	}


	void d3d12_surface::create_swap_chain(IDXGIFactory7* factory, ID3D12CommandQueue* command_queue, DXGI_FORMAT format)
	{
		assert(factory && command_queue);
		release();


		assert(window_.handle());
		assert(!swap_chain_);
		DXGI_SWAP_CHAIN_DESC1 desc{};
		desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		desc.BufferCount = frame_buffer_count;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.Flags = 0;
		desc.Format = to_non_srgb(format);
		desc.Height = window_.get_height();
		desc.Width = window_.get_width();
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Scaling = DXGI_SCALING_STRETCH;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.Stereo = false;

		IDXGISwapChain1* swap_chain{ nullptr };
		HWND hWnd{ (HWND)window_.handle() };
		DXCall(factory->CreateSwapChainForHwnd(command_queue, hWnd, &desc, nullptr, nullptr, &swap_chain));
		DXCall(factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));
		DXCall(swap_chain->QueryInterface(IID_PPV_ARGS(&swap_chain_)));
		core::release(swap_chain);

		current_bb_index = swap_chain_->GetCurrentBackBufferIndex();

		for (u32 i = 0; i < frame_buffer_count; ++i)
		{
			render_target_data_[i].rtv = core::rtv_heap().allocate();
		}

	}

	void d3d12_surface::present() const
	{
	}

	void d3d12_surface::resize()
	{
	}

	u32 d3d12_surface::width() const
	{
	}

	u32 d3d12_surface::height() const
	{
	}

	void d3d12_surface::release()
	{

	}
}
