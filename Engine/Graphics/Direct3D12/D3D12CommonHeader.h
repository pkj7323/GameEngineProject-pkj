#pragma once
#include "CommonHeader.h"
#include "Graphics/Renderer.h"

#include <dxgi1_6.h>
#include <d3d12.h>

#include <wrl.h> 
//스마트 포인터 같은건데 COM(포넌트)오브젝트 모델을 이용해 다렉을 만들었기때문에 사용함
//COM인터페이스의 포인터를 얻으면 릴리즈를 해줘야하는데 이것을 자동으로 해줌

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
