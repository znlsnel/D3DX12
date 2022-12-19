#pragma once

#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")

#include <windows.h>
#include <wrl.h>
#include <d3dcommon.h>
#include <exception>
#include <d3d12sdklayers.h>
#include <dxgi1_4.h>
#include <assert.h>

//#include <dxgi.h>
//#include <d3d12.h>
//#include <ppltasks.h>	// create_task의 경우
//#include <dxgidebug.h>


inline void ThrowIfFailed(HRESULT hr)
{
	// DirectX API 오류를 탐지하기 위해 이 줄에 중단점 설정
	if (FAILED(hr))
		throw std::exception();
}

DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;