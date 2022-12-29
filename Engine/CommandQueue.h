#pragma once
class SwapChain;
class DescriptorHeap;

class CommandQueue
{
public:
	~CommandQueue();
	void Init(ComPtr<ID3D12Device> device, 
			shared_ptr<SwapChain> swapChain);

	void WaitSync();

	void RenderBegin(const D3D12_VIEWPORT* vp,
					const D3D12_RECT* rect);

	void RenderEnd();

	ComPtr<ID3D12CommandQueue> GetCmdQueue() { return _cmdQueue; }
	ComPtr<ID3D12GraphicsCommandList> GetCmdList() { return _cmdList; }

private:
	// CommandQueue : DX12에 등장
	// 외주를 요청할 때, 하나씩 요청하면 비효율적
	// [외주 목록]에 일감을 차곡차곡 기록했다가 한 방에 요청하는 것
	ComPtr<ID3D12CommandQueue>			_cmdQueue;  // 여러 cmdList를 담음
	ComPtr<ID3D12CommandAllocator>		_cmdAlloc;  // 일감 할당자 (메모리 관리)
	ComPtr<ID3D12GraphicsCommandList>		_cmdList; // 일감 목록

	// Allocator가 메모리를 관리함 CommandQueue 클리어시 모든 데이터를 삭제하는 것이 라니라 vector처럼 capacity 값만 바꿈

	// Fence : 울타리(?) 
	// CPU / GPU 동기화를 위한 간단한 도구
	// 예를 들면 105번 작업이 끝날때 까지 대기하게 만드는.. 
	ComPtr<ID3D12Fence>					_fence;
	uint32									_fenceValue = 0;
	HANDLE									_fenceEvent = INVALID_HANDLE_VALUE;

	shared_ptr<SwapChain>		_swapChain;
};

