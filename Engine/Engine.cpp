#include "pch.h"
#include "Engine.h"
#include "Device.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "DescriptorHeap.h"

void Engine::Init(const WindowInfo& info)
{
	_window = info;
	ResizeWindow(info.width, info.height);

	// 그려질 화면 크기를 설정
	_viewport = { 0, 0, static_cast<FLOAT>(info.width), static_cast<FLOAT>(info.height), 0.0f, 1.0f };
	_scissorRect = CD3DX12_RECT(0, 0, info.width, info.height);

	_device = make_shared<Device>();
	_cmdQueue = make_shared<CommandQueue>();
	_swapChain = make_shared<SwapChain>();
	_descHeap = make_shared<DescriptorHeap>();

	_device->Init();
	_cmdQueue->Init(_device->GetDevice(),  _swapChain,  _descHeap);
	_swapChain->Init(info, _device->GetDXGI(), _cmdQueue->GetCmdQueue());
}

void Engine::Render()
{

}

void Engine::ResizeWindow(int32 width, int32 height)
{
	_window.width = width;
	_window.height = height;

	// :: 은 일반적인 함수가 아니라 라이브러리에서 제공하는 함수라는 것을 암시
	// ::를 빼도 되지만 결국  시인성 때문
	RECT rect = { 0, 0, _window.width, _window.height };
	::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

	// 창 생성 위치
	::SetWindowPos(_window.hWnd, 0, 100, 100, width, height, 0);
}