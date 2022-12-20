#pragma once

//#pragma comment(lib, "d3d12")
//#pragma comment(lib, "dxgi")

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")


#include <windows.h>
#include <wrl.h>
#include <d3dcommon.h>
#include <exception>
#include <d3d12sdklayers.h>
#include <dxgi1_4.h>
#include <assert.h>
#include "directx/d3dx12.h"


//#include <dxgi.h>
//#include <ppltasks.h>	// create_task�� ���
//#include <dxgidebug.h>


inline void ThrowIfFailed(HRESULT hr)
{
	// DirectX API ������ Ž���ϱ� ���� �� �ٿ� �ߴ��� ����
	if (FAILED(hr))
		throw std::exception();
}




int mClientWidth = 800;
int mClientHeight = 800;
int mCurrBackBuffer = 0;

bool m4xMsaaState = false;
unsigned int m4xMsaaQuality = 0;
const int SwapChainBufferCount = 2;




