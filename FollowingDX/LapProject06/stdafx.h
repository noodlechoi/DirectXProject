﻿// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>
#include <mmsystem.h>
// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <string>
#include <wrl.h>
#include <shellapi.h>

#include <d3d12.h>
#include <dxgi1_4.h>

#include <d3dcompiler.h>

#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

#include <dxgidebug.h>

// STL
#include <memory>
#include <deque>

using namespace DirectX;
using namespace DirectX::PackedVector;

using Microsoft::WRL::ComPtr;

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#pragma comment(lib, "dxguid.lib")

#pragma comment(lib, "winmm.lib")

#define FRAME_BUFFER_WIDTH	800
#define FRAME_BUFFER_HEIGHT 600


#define _WITH_SWAPCHAIN_FULLSCREEN_STATE

static inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr)) throw std::exception();
}

extern ID3D12Resource* CreateBufferResource(ID3D12Device*, ID3D12GraphicsCommandList*, void*, UINT, D3D12_HEAP_TYPE, D3D12_RESOURCE_STATES, ID3D12Resource**);