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
			//���塡�������׷��ȡ�ī�带��ã�´٣�
			for (u32 i{}; dxgi_factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
			                                                       IID_PPV_ARGS(&adapter)) != DXGI_ERROR_NOT_FOUND; ++i)
			{
				
			}
		}
	}



	bool initialize()
	{
		// ���� ���(���� ��� �׷���ī�� ����)�� ������ �˾Ƴ�����
		// �� ��Ͱ� ���� �� �� �ִ� ���� �ִ� �ֽ� ������ �������� �˾Ƴ�����
		// ID3D12Device�� �����ؾ���(�̰��� ���� �����)
		if (main_device)
		{
			shutdown();
		}
		u32 dxgi_factory_flags{ 0 };
#ifdef _DEBUG
		dxgi_factory_flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
		HRESULT hr{ S_OK };
		//CreateDXGIFactory2(0, __uuidof(IDXGIFactory7), (void**)dxgi_factory); �� ��ũ��
		hr = CreateDXGIFactory2(0,IID_PPV_ARGS(&dxgi_factory));
		if (FAILED(hr))
		{
			return failed_init();
		}

		// ���� ���(���� ��� �׷���ī�� ����)�� ������ �˾Ƴ�����
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
