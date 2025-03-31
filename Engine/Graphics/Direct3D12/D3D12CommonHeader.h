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

//assert that COM call to D3D API succeeded
#ifdef _DEBUG
#ifndef DXCall
#define DXCall(x)							\
	if (FAILED(x))							\
	{										\
		char line_num[32];					\
		sprintf_s(line_num,"%u",__LINE__);	\
		OutputDebugStringA("Error in :");	\
		OutputDebugStringA(__FILE__);		\
		OutputDebugStringA("\nLine: ");		\
		OutputDebugStringA(line_num);		\
		OutputDebugStringA("\n");			\
		OutputDebugStringA(#x);				\
		OutputDebugStringA("\n");			\
		__debugbreak();						\
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
	OutputDebugString(L"::D3D12 Object Created");
	OutputDebugString(name);
	OutputDebugString(L"\n");
}
#else
inline void NAME_D3D12_OBJECT(Microsoft::WRL::ComPtr<ID3D12Object> obj, const wchar_t* name)
{
	// Release mode: do nothing
}
#endif