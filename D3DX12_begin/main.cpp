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

#pragma region  COM

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
ComPtr<ID3D12Resource> mDepthStencilBuffer;

#pragma endregion



D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()
{
	return mDsvHeap->GetCPUDescriptorHandleForHeapStart();
}

D3D12_VIEWPORT vp;
D3D12_RECT mScissorRect;

DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

CD3DX12_HEAP_PROPERTIES properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
	mDepthStencilBuffer.Get(),
	D3D12_RESOURCE_STATE_COMMON,
	D3D12_RESOURCE_STATE_DEPTH_WRITE);





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

#pragma region ����, ���ٽ� ���ۿ� �� ����
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0; 
	depthStencilDesc.Width = mClientWidth;
	depthStencilDesc.Height = mClientHeight;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Format = mDepthStencilFormat;
	depthStencilDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	depthStencilDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear;
	optClear.Format = mDepthStencilFormat;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;
	

	ThrowIfFailed(md3dDevice->CreateCommittedResource(
		&properties, // �⺻ �� GPU�� ������ �ڿ��� ���
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&optClear,
		IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf())
	));

	// ��ü �ڿ��� �Ӹ� ���� 0�� ���� �����ڸ�
	// �ش� �ڿ��� �ȼ� ������ �����ؼ� �����Ѵ�.
	md3dDevice->CreateDepthStencilView(
		mDepthStencilBuffer.Get(),
		nullptr,
		DepthStencilView()
	);



	// �ڿ��� �ʱ� ���¿��� ���� ���۷� ����� �� �ִ� ���·� �����Ѵ�.
	mCommandList->ResourceBarrier(
		1,
		&resourceBarrier
	);



#pragma endregion

#pragma region ����Ʈ ����
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.Width = static_cast<float>(mClientWidth);
	vp.Height = static_cast<float>(mClientHeight);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	mCommandList->RSSetViewports(1, &vp);
#pragma endregion

#pragma region  ���� ���簢�� ����
	mScissorRect = { 0, 0, mClientWidth / 2, mClientHeight / 2 };
	mCommandList->RSSetScissorRects(1, &mScissorRect);
#pragma endregion

#pragma region ���� Ÿ�̸�
	// ��Ȯ�� �ð� ������ ���� ���

	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		__int64 countsPerSec;
		QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);

		double mSecondsPerCount = 1.0 / (double)countsPerSec;
	}

	// currTime �� A��� �ϰ� �� ������ QueryPerformanceCounter�Լ��� ��ԵǴ� ���� B��� �Ҷ�
	// � �۾��� �ɸ� �ð��� (B - A) * mSecondsPerCount ���̴�

#pragma endregion

#pragma region GameTimer Ŭ����
	// main.h�� �����
#pragma endregion

#pragma region	������ �� ��� �ð�
	// �ð��� �󸶳� �귶���� �˾ƾ���!
	// x�� n��° �������� �������Ҷ� ������ ���� Ÿ�̸� ���̰�
	// y�� �� ���� �����ӿ����� ���� Ÿ�̸� ���̶�� �ϸ�
	// x  - y�� �� ������ ������ ��� �ð��̶�� �� �� ����
	
	// main.h�� GameTimerŬ������ ��� TICK �Լ�
	
	// Tick�Լ� ȣ��
	// ----- main.h�� Run�Լ�
	// ----- GameTimer�� Reset�Լ�
#pragma endregion

#pragma region ��ü �ð�
	// ���� ���α׷��� ���۵� ���Ŀ� �帥 �ð��� ���� (������ �ð� ����)
	// ��ü �ð��� �����ϱ� ���� GameTime Ŭ������ ������ ���� ��� ������ �����
	// mBaseTime, mPausedTime, mStopTime

	// GameTimer Ŭ������ ����

	// GameTimer�� �ν��Ͻ��� �߰� �����Ͽ� ���� '�����ġ'�� ����ϴ� �͵� �󸸵��� ������
	// ���� ��� ��ź�� ��ȭ���� ���� �پ��� �� �� GameTimer�ν��Ͻ��� �����ϰ� TotalTime�� 5�ʰ� ������ ��ź�� �����ϴ� �ð��� �˹��ϴ� ������ Ȱ���� �� ���� ���̴�.
#pragma endregion

#pragma region d3dApp
	HINSTANCE mhAppInst = nullptr; // ���� ���α׷� �ڵ�
	HWND      mhMainWnd = nullptr; // �� â �ڵ�
	bool      mAppPaused = false;  // �Ͻ� ������ �����ΰ�?
	bool      mMinimized = false;  // �ּ�ȭ�� �����ΰ�?
	bool      mMaximized = false;  // �ִ�ȭ�� �����ΰ�?
	bool      mResizing = false;   // ũ�� ������ �׵θ��� ���� �ִ� �����ΰ�?
	bool      mFullscreenState = false;// ��üȭ�� Ȱ��ȭ ����

	// Set true to use 4X MSAA (?.1.8).  The default is false.
	bool      m4xMsaaState = false;    // 4X MSAA enabled
	UINT      m4xMsaaQuality = 0;      // quality level of 4X MSAA 
#pragma endregion
}
