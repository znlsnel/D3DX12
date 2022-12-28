#pragma once

// [��ȼ�]
// ���ָ� �ñ� �� �̷� ���� �������� ���� �Ѱ���� �ϴµ�,
// �ƹ� ���·γ� ��û�ϸ� �� �˾ƸԴ´�
// - ���� ���ҽ��� � �뵵�� ����ϴ��� �Ĳ��ϰ� ��� �Ѱ��ִ� �뵵

class DescriptorHeap
{
public:


private:
	ComPtr<ID3D12DescriptorHeap> _rtvHeap;
	uint32 _rtvHeapSize = 0;
	D3D12_CPU_DESCRIPTOR_HANDLE _rtvHandle[SWAP_CHAIN_BUFFER_COUNT];
	shared_ptr<class SwapChain>		_swapChain;
};

