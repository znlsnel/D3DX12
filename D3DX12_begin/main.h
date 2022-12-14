#pragma once

#include <windows.h>
#include <wrl.h>
#include <d3dcommon.h>
#include <d3d12sdklayers.h>

HRESULT WINAPI D3D12CreateDevice(
	IUnknown* pAdapter,
	D3D_FEATURE_LEVEL MinimumFeatureLevel,
	REFIID riid,
	void** ppDevice
);
