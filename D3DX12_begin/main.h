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
//#include <ppltasks.h>	// create_task�� ���
//#include <dxgidebug.h>


inline void ThrowIfFailed(HRESULT hr)
{
	// DirectX API ������ Ž���ϱ� ���� �� �ٿ� �ߴ��� ����
	if (FAILED(hr))
		throw std::exception();
}

DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;