#pragma once

//	-----------------------------------------------------------------------------------
//    |			descriptorHeap�� SwapChain�� ����	               |
//	-----------------------------------------------------------------------------------


// ��ȯ �罽
// [���� ����]
// - ���� ���� ���� �ִ� ��Ȳ�� ����
// - � �������� ��� ������� ��û��(descriptor)�� ������
// - GPU�� ������ ��� (����)
// - ����� �޾Ƽ� ȭ�鿡 �׷��ش�

// [���� �����]�� ��� ����?
// - � ����(Buffer)�� �׷��� �ǳ��޶�� ��Ź�غ���
// - Ư�� ���̸� ���� -> ó���� �ǳ��ְ� -> ������� �ش� ���̿� �޴´� OK
// - �츮 ȭ�鿡 Ư�� ����(���� �����) ������ش�

// [?]
// - �׷��� ȭ�鿡 ���� ����� ����ϴ� ���߿�, ���� ȭ�鵵 ���ָ� �ðܾ� ��
// - ���� ȭ�� ������� �̹� ȭ�� ��¿� �����
// - Ư�� ���̸� 2�� ����, �ϳ��� ���� ȭ���� �׷��ְ�, �ϳ��� ���� �ñ��...
// - Double Buffering!

// - buffer[1];
// ���� ȭ�� buffer[0]  <-> GPU �۾��� buffer[1] BackBuffer

class SwapChain
{
public:
	void Init(const WindowInfo& info, 
			ComPtr<ID3D12Device> device,
			ComPtr<IDXGIFactory> dxgi, 
			ComPtr<ID3D12CommandQueue> cmdQueue);

	void Present();
	void SwapIndex();
	
	ComPtr<IDXGISwapChain> GetSwapChain() { 
		return _swapChain; }

	uint32 GetBackBufferIndex() { 
		return _backBufferIndex; }



private:
	void CreateSwapChain(const WindowInfo& info,
		ComPtr<IDXGIFactory> dxgi,
		ComPtr<ID3D12CommandQueue> cmdQueue);

private:
	ComPtr<IDXGISwapChain>				_swapChain;
	uint32				 _backBufferIndex = 0; // ���� backBuffer�� Index��ȣ
};

