#pragma once
#include "CommonHeader.h"
#include "Graphics/Renderer.h"

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
		char line_num[256];					\
		sprintf_s(line_num,"%u",__LINE__);	\
		OutputDebugString("Error in :");	\
		OutputDebugString(__FILE__);		\
		OutputDebugString("\nLine: ");		\
		OutputDebugString(line_num);		\
		OutputDebugString(#x);				\
		OutputDebugString("\n");			\
		__debugbreak();						\
	}
#endif
#else
#ifndef DXCall
#define DXCall(x) x
#endif
#endif // DEBUG
