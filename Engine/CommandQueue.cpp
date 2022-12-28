#include "pch.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "DescriptorHeap.h"

CommandQueue::~CommandQueue()
{
	// event�� �׻� �������� ������� 
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
	
	// - D3D12_COMMAND_LIST_TYPE_DIRECT : GPU�� ���� �����ϴ� ��� ���
	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, 
									IID_PPV_ARGS(&_cmdAlloc));

	// GPU�� �ϳ��� �ý��ۿ����� 0����
	// DIRECT
	// Allocator
	// �ʱ� ���� (�׸��� ����� nullptr ����)
	device->CreateCommandList(0, 
								D3D12_COMMAND_LIST_TYPE_DIRECT, 
								_cmdAlloc.Get(), 
								nullptr, IID_PPV_ARGS(&_cmdList));

	// CommandList�� Close / Open ���°� �ִµ�
	// Open ���¿��� Command�� �ִٰ� Close�� ���� �����ϴ� ����
	_cmdList->Close();

	// CreateFence
	// - CPU�� GPU�� ����ȭ �������� ���δ�
	device->CreateFence(0, 
						D3D12_FENCE_FLAG_NONE, 
						IID_PPV_ARGS(&_fence));

	_fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void CommandQueue::WaitSync()
{
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