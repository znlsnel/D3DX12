#include "main.h"
using namespace Microsoft::WRL;

// Rtv : Render Target view
// Dsv : depth , stencil view 
// Cbv : Constant buffer view 
// Srv : Shader resource view
// Uav : Unordered access view (���� ���� ���� )
int mRtvDescriptorSize;
int mDsvDescriptorSize;
int mCbvSrvDescriptorSize;

// stencill buffer�� ?
// Ư�� �ȼ����� �ĸ���ۿ� ��ϵ��� �ʵ��� �ϴ� ����
// ex) �׸���, �ſ� �������� ����

DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

ComPtr<ID3D12Device> md3dDevice;// = ComPtr<ID3D12Device>();
ComPtr<IDXGIFactory4> mdxgiFactory;
ComPtr<ID3D12Fence1> mFence;
ComPtr<IDXGISwapChain> mSwapChain;
ComPtr<ID3D12CommandQueue> mCommandQueue; // ��� ��⿭
ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc; // ��� �Ҵ���
ComPtr<ID3D12GraphicsCommandList> mCommandList; // ��� ���
ComPtr<ID3D12DescriptorHeap> mRtvHeap; // render target view Heap
ComPtr<ID3D12DescriptorHeap> mDsvHeap; // Deap Stencil View Heap
ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];

int main() 
{
#pragma region ��ġ ����

#if defined(DEBUG) || defined(_DEBUG)
	{ // ������� Ȱ��ȭ
		ComPtr<ID3D12Debug> debugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
		debugController->EnableDebugLayer();
	}
#endif


	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&mdxgiFactory)));

	// �ϵ���� ����͸� ��Ÿ���� ��ġ�� ����
	HRESULT hardwareResult = D3D12CreateDevice(
		nullptr,
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&md3dDevice));


	// �����ߴٸ� WARP ����͸� ��Ÿ���� ��ġ�� ����
	if (FAILED(hardwareResult))
	{
		ComPtr<IDXGIAdapter> pWarpAdapter;// = ComPtr<IDXGIAdapter>();
		ThrowIfFailed(mdxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));

		ThrowIfFailed(D3D12CreateDevice(
			pWarpAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&md3dDevice)
		));
	}

#pragma endregion

#pragma region  DescriptorSize
	ThrowIfFailed(md3dDevice->CreateFence(
		0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)));

	mRtvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV
	);
	mDsvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_DSV
	);
	mCbvSrvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
		);
#pragma endregion
	   
#pragma region 4X MSAA ǰ�� ���� ���� ����
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = mBackBufferFormat;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;

	ThrowIfFailed(md3dDevice->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&msQualityLevels,
		sizeof(msQualityLevels)));
	m4xMsaaQuality = msQualityLevels.NumQualityLevels;
	assert(m4xMsaaQuality > 0 && "Unexpected MSAA quality level.");
	
#pragma endregion

#pragma region ��� ��⿭�� ��� ��� ����


	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	// CommandQueue ��� ��⿭
	ThrowIfFailed(md3dDevice->CreateCommandQueue(
		&queueDesc, IID_PPV_ARGS(&mCommandQueue)));

	// CommandAllocator ��� �Ҵ���
	ThrowIfFailed(md3dDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(mDirectCmdListAlloc.GetAddressOf())));

	// CommandList ��� ���
	ThrowIfFailed(md3dDevice->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		mDirectCmdListAlloc.Get(),
		nullptr, // �׸��� ��� �����Ұ� �����Ƿ� nullptr
		IID_PPV_ARGS(mCommandList.GetAddressOf())));

	// ��� ����� ó�� ȣ���� �� Resetd�� ȣ���ϴµ�
	// Reset�� ȣ���Ϸ��� ��� ����� �����־�� ��
	mCommandList->Close();
	

#pragma endregion

#pragma region  ��ȯ �罽�� ������ ����

	mSwapChain.Reset();

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = mClientWidth;
	sd.BufferDesc.Height = mClientHeight;
	sd.BufferDesc.RefreshRate.Denominator = 1; // 1�п�
	sd.BufferDesc.RefreshRate.Numerator = 60; // 60��
	sd.BufferDesc.Format = mBackBufferFormat;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // �ȼ� ���� ������� ���
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // �����ϸ� ����X (Ȯ�� ����, ���÷��� �߾ӿ� ��ġ�ϰ� �ϴµ��� ����� ����)
	sd.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	sd.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	sd.Windowed = true; // â���
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	//�� �÷��׸� ����Ͽ� ��Ʈ ��� ���� ���� ����IDXGISwapChain1::P resent1�� ȣ�� �� DXGI�� �� ������ �������� �����ϵ��� ����
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	// â��忡�� ��ü ȭ�� ���� ��ȯ�ϸ� ���� ���α׷��� â ũ��� ��ġ�ϰ� ���÷��� ��尡 �����

	ThrowIfFailed(mdxgiFactory->CreateSwapChain(
		mCommandQueue.Get(), // ��� ��⿭�� �����͸� ��ȯ
		&sd,
		mSwapChain.GetAddressOf() // �������� �ּҰ��� ��ȯ
	));

	
#pragma endregion

#pragma region ������ �� ����

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = SwapChainBufferCount; //  Heap desc ��
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;

	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
		&rtvHeapDesc, IID_PPV_ARGS(mRtvHeap.GetAddressOf())
	));

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	rtvHeapDesc.NumDescriptors = 1; //  Heap desc ��
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;

	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
		&dsvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())
	));
	
#pragma endregion

#pragma region ���� ��� ��(RTV) ����
	
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(
		mRtvHeap->GetCPUDescriptorHandleForHeapStart());
	
	for (UINT i = 0; i < SwapChainBufferCount; i++)
	{
		// ��ȯ ����� i��° ���۸� ��´�.!
		ThrowIfFailed(mSwapChain->GetBuffer(
			i, IID_PPV_ARGS(&mSwapChainBuffer[i]) ));

		// �� ���ۿ� ���� RTV�� �����Ѵ�.!
		md3dDevice->CreateRenderTargetView(
			mSwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);

		// ���� ���� �׸����� �Ѿ��.
		rtvHeapHandle.Offset(1, mRtvDescriptorSize);
	}
#pragma endregion



}
