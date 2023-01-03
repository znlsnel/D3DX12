#include "pch.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "Engine.h"
CommandQueue::~CommandQueue()
{
	// event�� �׻� �������� ������� 
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
	// - CPU�� GPU�� ����ȭ �������� ���δ�
	device->CreateFence(0, 
						D3D12_FENCE_FLAG_NONE, 
						IID_PPV_ARGS(&_fence));

	_fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void CommandQueue::WaitSync()
{
	// �ϰ��� �� ������ ���� ������ִ� �ڵ�

	// �潺 ���� ������ �̵��Ͽ� �� �潺 ����Ʈ���� ����� ǥ���մϴ�.
	// fence�� ��ȣ�� ������ ����
	_fenceValue++;

	// ��� ��⿭�� ����� �߰��Ͽ� �� �潺 ����Ʈ�� ����
	// GPU Ÿ�Ӷ��ο� �����Ƿ� GPU�� �Ϸ�� ������ �� �潺 ����Ʈ�� �����ȵ�
	//  Signal() ������ ��� ����� ó���մϴ�.
	_cmdQueue->Signal(_fence.Get(), _fenceValue);

	// GPU�� �� �潺 ����Ʈ���� ����� �Ϸ��� ������ ���
	if (_fence->GetCompletedValue() < _fenceValue)
	{
		// GPU�� ���� �潺�� �����ϸ� �̺�Ʈ�� �߻�
		_fence->SetEventOnCompletion(_fenceValue, _fenceEvent);

		// GPU�� ���� �潺 �̺�Ʈ�� ������ ������ ��ٸ��ϴ�.
		::WaitForSingleObject(_fenceEvent, INFINITE);
	}
}

// ���Ӱ� �׸� buffer�� ������
void CommandQueue::RenderBegin(const D3D12_VIEWPORT* vp, const D3D12_RECT* rect)
{
	_cmdAlloc->Reset();
	_cmdList->Reset(_cmdAlloc.Get(), nullptr);

	// ���� ���۸� �ĸ�����
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		_swapChain->GetBackRTVBuffer().Get(),
		D3D12_RESOURCE_STATE_PRESENT, // ȭ�� ��� 
		D3D12_RESOURCE_STATE_RENDER_TARGET); // ���� �����

	// �����ϰڴ�!
	_cmdList->SetGraphicsRootSignature(ROOT_SIGNATURE.Get());
	GEngine->GetConstantBuffer()->Clear();
	GEngine->GetTableDescHeap()->Clear();

	ID3D12DescriptorHeap* descHeap = DESC_HEAP.Get();
	_cmdList->SetDescriptorHeaps(1, &descHeap);

	_cmdList->ResourceBarrier(1, &barrier);

	// Set the viewport and scissor rect.  This needs to be reset whenever the command list is reset.
	_cmdList->RSSetViewports(1, vp);
	_cmdList->RSSetScissorRects(1, rect);

	// Specify the buffers we are going to render to.
	D3D12_CPU_DESCRIPTOR_HANDLE backBufferView = _swapChain->GetBackRTVHandle();
	_cmdList->ClearRenderTargetView(backBufferView, Colors::LightSteelBlue, 0, nullptr);

	D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView = GEngine->GetDepthStencilBuffer()->GetDSVCpuHandle();
	_cmdList->OMSetRenderTargets(1, &backBufferView, FALSE, &depthStencilView);

	_cmdList->ClearDepthStencilView(depthStencilView, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

// ��¥ �׷��޶�� ��û�ϰ� �Ǵ� �κ�
void CommandQueue::RenderEnd()
{	// �ĸ� ���۸� ��������
	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		_swapChain->GetBackRTVBuffer().Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, // ���� �����
		D3D12_RESOURCE_STATE_PRESENT); // ȭ�� ���

	_cmdList->ResourceBarrier(1, &barrier);
	_cmdList->Close();

	// Ŀ�ǵ� ����Ʈ ����
	ID3D12CommandList* cmdListArr[] = { _cmdList.Get() };
	_cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

	_swapChain->Present();

	// Wait until frame commands are complete.  This waiting is inefficient and is
	// done for simplicity.  Later we will show how to organize our rendering code
	// so we do not have to wait per frame.
	WaitSync();

	// ���� ���ۿ� �ĸ� ���۸� �ٲ�!
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