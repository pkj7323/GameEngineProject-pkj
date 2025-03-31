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
		IDXGIAdapter4* determine_main_adapter()
		{
			IDXGIAdapter4* adapter{ nullptr };
			//가장　좋은　그래픽　카드를　찾는다．
			for (u32 i{}; dxgi_factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
			                                                       IID_PPV_ARGS(&adapter)) != DXGI_ERROR_NOT_FOUND; ++i)
			{
				if (SUCCEEDED(D3D12CreateDevice(adapter, minimum_feature_level, __uuidof(ID3D12Device), nullptr)))
				{
					return adapter;
				}
				release(adapter);
			}

			return nullptr; // 유능한 그래픽 카드를 찾지 못함
		}
		D3D_FEATURE_LEVEL get_max_feature_level(IDXGIAdapter4* adapter)
		{
			constexpr std::array<D3D_FEATURE_LEVEL, 4> feature_levels
			{
				D3D_FEATURE_LEVEL_11_0,
				D3D_FEATURE_LEVEL_11_1,
				D3D_FEATURE_LEVEL_12_0,
				D3D_FEATURE_LEVEL_12_1,
			};

			D3D12_FEATURE_DATA_FEATURE_LEVELS feature_levels_info{};
			feature_levels_info.NumFeatureLevels = feature_levels.size();
			feature_levels_info.pFeatureLevelsRequested = feature_levels.data();

			ComPtr<ID3D12Device> device;
			DXCall(D3D12CreateDevice(adapter, minimum_feature_level, IID_PPV_ARGS(&device)));
			DXCall(device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &feature_levels_info, sizeof(feature_levels_info)));
			return feature_levels_info.MaxSupportedFeatureLevel;
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
		//디버그 레이어 활성화 하려면 컴퓨터에서 설정->선택적 기능 추가->그래픽 도구 가 추가되어있어야함
		{
			ComPtr<ID3D12Debug3> debug_interface;
			DXCall(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_interface)));
			debug_interface->EnableDebugLayer();
			dxgi_factory_flags |= DXGI_CREATE_FACTORY_DEBUG;
		}
#endif
		HRESULT hr{ S_OK };
		//CreateDXGIFactory2(0, __uuidof(IDXGIFactory7), (void**)dxgi_factory); 의 매크로
		hr = CreateDXGIFactory2(0,IID_PPV_ARGS(&dxgi_factory));
		if (FAILED(hr))
		{
			return failed_init();
		}

		// 무슨 어뎁터(예를 들어 그래픽카드 같은)를 쓰는지 알아내야함
		ComPtr<IDXGIAdapter4> main_adapter;
		main_adapter.Attach(determine_main_adapter());
		if (not main_adapter)
		{
			return failed_init();
		}

		D3D_FEATURE_LEVEL max_feature_level{ get_max_feature_level(main_adapter.Get()) };
		assert(max_feature_level >= minimum_feature_level);
		if (max_feature_level < minimum_feature_level)
		{
			return failed_init();
		}

		DXCall(hr = D3D12CreateDevice(main_adapter.Get(), max_feature_level, IID_PPV_ARGS(&main_device)));
		if (FAILED(hr))
		{
			return failed_init();
		}

		NAME_D3D12_OBJECT(main_device, L"Main Device");
#ifdef _DEBUG
		{
			ComPtr<ID3D12InfoQueue> info_queue;
			DXCall(main_device->QueryInterface(IID_PPV_ARGS(&info_queue)));

			info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true); //오류
			info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true); //경고
			info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true); //에러
			
		}
#endif
		return true;
	}
	void shutdown()
	{
		release(dxgi_factory);

#ifdef _DEBUG
		{
			{
				ComPtr<ID3D12InfoQueue> info_queue;
				DXCall(main_device->QueryInterface(IID_PPV_ARGS(&info_queue)));

				info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, false); //오류
				info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, false); //경고
				info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, false); //에러
			}

			ComPtr<ID3D12DebugDevice2> debug_device;
			DXCall(main_device->QueryInterface(IID_PPV_ARGS(&debug_device)));
			release(main_device);
			DXCall(debug_device->ReportLiveDeviceObjects(
				D3D12_RLDO_SUMMARY | D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL));
		}
#endif

		release(main_device);
	}
}
