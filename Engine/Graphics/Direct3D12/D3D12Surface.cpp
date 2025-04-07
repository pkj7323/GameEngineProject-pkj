#include "D3D12Surface.h"
#include "D3D12Core.h"

namespace primal::graphics::d3d12
{
    namespace
    {

        constexpr DXGI_FORMAT to_non_srgb(DXGI_FORMAT format)
        {
            if (format == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB) return DXGI_FORMAT_R8G8B8A8_UNORM;

            return format;
        }

    } // anonymous namespace

    void d3d12_surface::create_swap_chain(IDXGIFactory7* factory, ID3D12CommandQueue* cmd_queue, DXGI_FORMAT format)
    {
        assert(factory && cmd_queue);
        release();

        if (SUCCEEDED(factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allow_tearing_, sizeof(u32))) && allow_tearing_)
        {
            present_flags_ = DXGI_PRESENT_ALLOW_TEARING;
        }

        DXGI_SWAP_CHAIN_DESC1 desc{};
        desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        desc.BufferCount = frame_buffer_count;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.Flags = allow_tearing_ ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
        desc.Format = to_non_srgb(format);
        desc.Height = window_.get_height();
        desc.Width = window_.get_width();
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Scaling = DXGI_SCALING_STRETCH;
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        desc.Stereo = false;


        IDXGISwapChain1* swap_chain;
        HWND hwnd{ (HWND)window_.handle() };
        if (!hwnd)
        {
            __debugbreak();
        }
        DXCall(factory->CreateSwapChainForHwnd(cmd_queue, hwnd, &desc, nullptr, nullptr, &swap_chain));
        DXCall(factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER));
        DXCall(swap_chain->QueryInterface(IID_PPV_ARGS(&swap_chain_)));
        core::release(swap_chain);

        current_bb_index_ = swap_chain_->GetCurrentBackBufferIndex();

        for (u32 i{ 0 }; i < frame_buffer_count; ++i)
        {
            render_target_data_[i].rtv = core::rtv_heap().allocate();
        }

        finalize();
    }

    void d3d12_surface::present() const
    {
        assert(swap_chain_);
        DXCall(swap_chain_->Present(0, present_flags_));
        current_bb_index_ = swap_chain_->GetCurrentBackBufferIndex();
    }

    void d3d12_surface::resize()
    {

    }

    void d3d12_surface::finalize()
    {
        // create RTVs for back-buffers
        for (u32 i{ 0 }; i < frame_buffer_count; ++i)
        {
            render_target_data& data{ render_target_data_[i] };
            assert(!data.resource);
            DXCall(swap_chain_->GetBuffer(i, IID_PPV_ARGS(&data.resource)));
            D3D12_RENDER_TARGET_VIEW_DESC desc{};
            desc.Format = core::default_render_target_format();
            desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
            core::device()->CreateRenderTargetView(data.resource, &desc, data.rtv.cpu);
        }

        DXGI_SWAP_CHAIN_DESC desc{};
        DXCall(swap_chain_->GetDesc(&desc));
        const u32 width{ desc.BufferDesc.Width };
        const u32 height{ desc.BufferDesc.Height };
        assert(window_.get_width() == width && window_.get_height() == height);

        // set viewport and scissor rect
        viewport_.TopLeftX = 0.f;
        viewport_.TopLeftY = 0.f;
        viewport_.Width = (float)width;
        viewport_.Height = (float)height;
        viewport_.MinDepth = 0.f;
        viewport_.MaxDepth = 1.f;

        scissor_rect_ = { 0, 0, (i32)width, (i32)height };
    }

    void d3d12_surface::release()
    {
        for (u32 i{ 0 }; i < frame_buffer_count; ++i)
        {
            render_target_data& data{ render_target_data_[i] };
            core::release(data.resource);
            core::rtv_heap().free(data.rtv);
        }

        core::release(swap_chain_);
    }

}