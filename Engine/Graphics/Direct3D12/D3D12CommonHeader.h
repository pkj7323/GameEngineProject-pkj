#pragma once
#include "CommonHeader.h"
#include "../Graphics/Renderer.h"

#include <dxgi1_6.h>
#include <d3d12.h>

#include <wrl.h> 
//����Ʈ ������ �����ǵ� COM(����Ʈ)������Ʈ ���� �̿��� �ٷ��� ������⶧���� �����
//COM�������̽��� �����͸� ������ ����� ������ϴµ� �̰��� �ڵ����� ����

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")
namespace primal::graphics::d3d12
{
	constexpr u32 frame_buffer_count{ 3 };
}
//assert that COM call to D3D API succeeded
#define HR_TO_STRING(x) #x
#ifdef _DEBUG
#ifndef DXCall
inline HRESULT DXCall(HRESULT hr)
{
	if (FAILED(hr))
	{										
		char line_num[32];					
		sprintf_s(line_num, "%u", __LINE__);
		OutputDebugStringA("Error in :");	
		OutputDebugStringA(__FILE__);		
		OutputDebugStringA("\nLine: ");		
		OutputDebugStringA(line_num);		
		OutputDebugStringA("\n");			
		OutputDebugStringA(HR_TO_STRING(hr));
		OutputDebugStringA("\n");			
		__debugbreak();						
	}
	return hr;
}
#endif
#else
#ifndef DXCall
#define DXCall(x) x
#endif
#endif // DEBUG

#ifdef _DEBUG
inline void NAME_D3D12_OBJECT(Microsoft::WRL::ComPtr<ID3D12Object> obj, const wchar_t* name)
{
	obj->SetName(name);
	OutputDebugString(L"::D3D12 Object Created ");
	OutputDebugString(name);
	OutputDebugString(L"\n");
}
inline void NAME_D3D12_OBJECT_INDEXED(Microsoft::WRL::ComPtr<ID3D12Object> obj, u32 index, const wchar_t* name)
{
	wchar_t full_name[256];
	if (0 < swprintf_s(full_name, L"%s[%u]", name, index))
	{
		obj->SetName(full_name);
		OutputDebugString(L"::D3D12 Object Created");
		OutputDebugString(full_name);
		OutputDebugString(L"\n");
	}
	
}
#else
inline void NAME_D3D12_OBJECT(Microsoft::WRL::ComPtr<ID3D12Object> obj, const wchar_t* name)
{
	// Release mode: do nothing
}
inline void NAME_D3D12_OBJECT_INDEXED(Microsoft::WRL::ComPtr<ID3D12Object> obj, u32 index, const wchar_t* name)
{
	// Release mode: do nothing
}
#endif