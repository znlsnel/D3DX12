#include "pch.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "Engine.h"
CommandQueue::~CommandQueue()
{
	// event는 항상 마지막에 꺼줘야함 
	::CloseHandle(_fenceEvent);
}

void CommandQueue::Init(ComPtr<ID3D12Device>		device, 
						shared_ptr<SwapChain>		swapChain)
{
	_swapChain = swapChain;

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&_cmdQueue));
	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, 
									IID_PPV_ARGS(&_cmdAlloc));
	device->CreateCommandList(0, 
								D3D12_COMMAND_LIST_TYPE_DIRECT, 
								_cmdAlloc.Get(), 
								nullptr, 
								IID_PPV_ARGS(&_cmdList));
	_cmdList->Close();

	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
									IID_PPV_ARGS(&_resCmdAlloc));

	device->CreateCommandList(0,
								D3D12_COMMAND_LIST_TYPE_DIRECT,
								_resCmdAlloc.Get(),
								nullptr, 
								IID_PPV_ARGS(&_resCmdList));

	// CreateFence
	// - CPU와 GPU의 동기화 수단으로 쓰인다
	device->CreateFence(0, 
						D3D12_FENCE_FLAG_NONE, 
						IID_PPV_ARGS(&_fence));

	_fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void CommandQueue::WaitSync()
{
	// 일감이 다 끝날때 까지 대기해주는 코드

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

// 새롭게 그릴 buffer를 가져옴
void CommandQueue::RenderBegin(const D3D12_VIEWPORT* vp, const D3D12_RECT* rect)
{
	_cmdAlloc->Reset();
	_cmdList->Reset(_cmdAlloc.Get(), nullptr);

	int8 backIndex = _swapChain->GetBackBufferIndex();
	// 전면 버퍼를 후면으로
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->GetRTTexture(backIndex)->GetTex2D().Get(),

		D3D12_RESOURCE_STATE_PRESENT, // 화면 출력
		D3D12_RESOURCE_STATE_RENDER_TARGET); // 외주 결과물

	// 서명하겠다!
	_cmdList->SetGraphicsRootSignature(ROOT_SIGNATURE.Get());

	GEngine->GetConstantBuffer(CONSTANT_BUFFER_TYPE::TRANSFORM)->Clear();
	GEngine->GetConstantBuffer(CONSTANT_BUFFER_TYPE::MATERIAL)->Clear();

	GEngine->GetTableDescHeap()->Clear();

	ID3D12DescriptorHeap* descHeap = DESC_HEAP.Get();
	_cmdList->SetDescriptorHeaps(1, &descHeap);

	_cmdList->ResourceBarrier(1, &barrier);

	// Set the viewport and scissor rect.  This needs to be reset whenever the command list is reset.
	_cmdList->RSSetViewports(1, vp);
	_cmdList->RSSetScissorRects(1, rect);

}

// 진짜 그려달라고 요청하게 되는 부분
void CommandQueue::RenderEnd()
{	// 후면 버퍼를 전면으로
	int8 backIndex = _swapChain->GetBackBufferIndex();

	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->GetRTTexture(backIndex)->GetTex2D().Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, // 외주 결과물
		D3D12_RESOURCE_STATE_PRESENT); // 화면 출력

	_cmdList->ResourceBarrier(1, &barrier);
	_cmdList->Close();

	// 커맨드 리스트 수행
	ID3D12CommandList* cmdListArr[] = { _cmdList.Get() };
	_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

	_swapChain->Present();

	// Wait until frame commands are complete.  This waiting is inefficient and is
	// done for simplicity.  Later we will show how to organize our rendering code
	// so we do not have to wait per frame.
	WaitSync();

	// 전면 버퍼와 후면 버퍼를 바꿈!
	_swapChain->SwapIndex();
}

void CommandQueue::FlushResourceCommandQueue()
{
	_resCmdList->Close();

	ID3D12CommandList* cmdListArr[] = { _resCmdList.Get() };
	_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

	WaitSync();

	_resCmdAlloc->Reset();
	_resCmdList->Reset(_resCmdAlloc.Get(), nullptr);
}