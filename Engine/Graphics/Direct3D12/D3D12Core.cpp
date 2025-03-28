#include "D3D12Core.h"
using namespace Microsoft::WRL;
namespace primal::graphics::d3d12::core
{
	namespace 
	{
		ID3D12Device8* main_device{ nullptr };
		IDXGIFactory7* dxgi_factory{ nullptr };
		constexpr D3D_FEATURE_LEVEL minimum_feature_level{ D3D_FEATURE_LEVEL_11_0 };

		bool failed_init()
		{
			shutdown();
			return false;
		}
		IDXGIAdapter4* determin_main_adapter()
		{
			IDXGIAdapter4* adapter{ nullptr };
			//가장　좋은　그래픽　카드를　찾는다．
			for (u32 i{}; dxgi_factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
			                                                       IID_PPV_ARGS(&adapter)) != DXGI_ERROR_NOT_FOUND; ++i)
			{
				
			}
		}
	}



	bool initialize()
	{
		// 무슨 어뎁터(예를 들어 그래픽카드 같은)를 쓰는지 알아내야함
		// 그 어뎁터가 제공 할 수 있는 가장 최대 최신 버전이 무엇인지 알아내야함
		// ID3D12Device를 생성해야함(이것은 가상 어뎁터임)
		if (main_device)
		{
			shutdown();
		}
		u32 dxgi_factory_flags{ 0 };
#ifdef _DEBUG
		dxgi_factory_flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
		HRESULT hr{ S_OK };
		//CreateDXGIFactory2(0, __uuidof(IDXGIFactory7), (void**)dxgi_factory); 의 매크로
		hr = CreateDXGIFactory2(0,IID_PPV_ARGS(&dxgi_factory));
		if (FAILED(hr))
		{
			return failed_init();
		}

		// 무슨 어뎁터(예를 들어 그래픽카드 같은)를 쓰는지 알아내야함
		ComPtr<IDXGIAdapter1> main_adapter;
		main_adapter.Attach(determin_main_adapter());
		if (!main_adapter)
		{
			return failed_init();
		}




	}
	void shutdown()
	{}
}
