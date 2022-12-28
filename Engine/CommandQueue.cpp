#include "pch.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "DescriptorHeap.h"

CommandQueue::~CommandQueue()
{
	// event는 항상 마지막에 꺼줘야함 
	::CloseHandle(_fenceEvent);
}

void CommandQueue::Init(ComPtr<ID3D12Device>		device, 
						shared_ptr<SwapChain>		swapChain, 
						shared_ptr<DescriptorHeap>	descHeap)
{
	_swapChain = swapChain;
	_descHeap = descHeap;

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&_cmdQueue));
	
	// - D3D12_COMMAND_LIST_TYPE_DIRECT : GPU가 직접 실행하는 명령 목록
	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, 
									IID_PPV_ARGS(&_cmdAlloc));

	// GPU가 하나인 시스템에서는 0으로
	// DIRECT
	// Allocator
	// 초기 상태 (그리기 명령은 nullptr 지정)
	device->CreateCommandList(0, 
								D3D12_COMMAND_LIST_TYPE_DIRECT, 
								_cmdAlloc.Get(), 
								nullptr, IID_PPV_ARGS(&_cmdList));

	// CommandList는 Close / Open 상태가 있는데
	// Open 상태에서 Command를 넣다가 Close한 다음 제출하는 개념
	_cmdList->Close();

	// CreateFence
	// - CPU와 GPU의 동기화 수단으로 쓰인다
	device->CreateFence(0, 
						D3D12_FENCE_FLAG_NONE, 
						IID_PPV_ARGS(&_fence));

	_fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void CommandQueue::WaitSync()
{
	// 펜스 값을 앞으로 이동하여 이 펜스 포인트까지 명령을 표시합니다.
	// fence는 번호를 가지고 있음
	_fenceValue++;

	// 명령 대기열에 명령을 추가하여 새 펜스 포인트를 설정
	// GPU 타임라인에 있으므로 GPU가 완료될 때까지 새 펜스 포인트가 설정안됨
	//  Signal() 이전에 모든 명령을 처리합니다.
	_cmdQueue->Signal(_fence.Get(), _fenceValue);

	// GPU가 이 펜스 포인트까지 명령을 완료할 때까지 대기
	if (_fence->GetCompletedValue() < _fenceValue)
	{
		// GPU가 현재 펜스에 도달하면 이벤트를 발생
		_fence->SetEventOnCompletion(_fenceValue, _fenceEvent);

		// GPU가 현재 펜스 이벤트에 도달할 때까지 기다립니다.
		::WaitForSingleObject(_fenceEvent, INFINITE);
	}
}