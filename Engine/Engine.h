#pragma once
class Engine
{
public:

	void Init(const WindowInfo& info);
	void Render();
	
	void ResizeWindow(int32 width, int32 height);

private:
	//  �׷��� ȭ�� ũ�� ����
	WindowInfo _window;
	D3D12_VIEWPORT _viewport = {};

		/*
		typedef struct D3D12_VIEWPORT
		{
			FLOAT TopLeftX;
			FLOAT TopLeftY;
			FLOAT Width;
			FLOAT Height;
			FLOAT MinDepth;
			FLOAT MaxDepth;
		} 	D3D12_VIEWPORT;
		*/

	D3D12_RECT _scissorRect = {};

	shared_ptr<class Device> _device;
	shared_ptr<class CommandQueue> _cmdQueue;
	shared_ptr<class SwapChain> _swapChain;
	shared_ptr<class DescriptorHeap> _descHeap;

};

