#pragma once

// 각종 include
#include <windows.h>
#include <tchar.h>
#include <memory>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <map>
using namespace std;

#include "d3dx12.h"
#include <d3d12.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace Microsoft::WRL;


// 각종 lib
#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "dxguid")
#pragma comment(lib, "d3dcompiler")

// 각종 typedef
using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;
using Vec2 = XMFLOAT2;
using Vec3 = XMFLOAT3;
using Vec4 = XMFLOAT4;
using Matrix = XMMATRIX;

enum class CBV_REGISTER
{
	b0,
	b1,
	b2,
	b3,
	b4,

	END // 0 ~ 4까지니까 END는 5 -- 즉 갯수를 나타냄
};

enum
{
	SWAP_CHAIN_BUFFER_COUNT = 2,
	CBV_REGISTER_COUNT = CBV_REGISTER::END,
	REGISTER_COUNT = CBV_REGISTER::END, // 총 레지스터 갯수
};

struct WindowInfo
{
	HWND hWnd;
	int32 width;
	int32 height;
	bool windowed; // 창모드 or 전체화면
};

struct Vertex
{
	Vec3 pos; // float가 3개 ( x, y, z )
	Vec4 color; // float가 4개 ( r, g, b, a )
};

struct Transform
{
	Vec4 offset;
};
#define DEVICE GEngine->GetDevice()->GetDevice()
#define CMD_LIST GEngine->GetCmdQueue()->GetCmdList()
#define ROOT_SIGNATURE GEngine->GetRootSignature()->GetSignature()
#define DESC_HEAP GEngine->GetTableDescHeap()->GetDescriptorHeap()

//  unique_ptr<Engine> GEngine 라는게 나올거다~ 라는거
extern unique_ptr<class Engine> GEngine;
