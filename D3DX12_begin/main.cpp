#include "main.h"
using namespace Microsoft::WRL;

// Rtv : Render Target view
// Dsv : depth , stencil view 
// Cbv : Constant buffer view 
// Srv : Shader resource view
// Uav : Unordered access view (순서 없는 접근 )
int mRtvDescriptorSize;
int mDsvDescriptorSize;
int mCbvSrvDescriptorSize;

// stencill buffer란 ?
// 특정 픽셀들이 후면버퍼에 기록되지 않도록 하는 버퍼
// ex) 그림자, 거울 랜더링시 사용됨

#pragma region  COM

ComPtr<ID3D12Device> md3dDevice;// = ComPtr<ID3D12Device>();
ComPtr<IDXGIFactory4> mdxgiFactory;
ComPtr<ID3D12Fence1> mFence;
ComPtr<IDXGISwapChain> mSwapChain;
ComPtr<ID3D12CommandQueue> mCommandQueue; // 명령 대기열
ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc; // 명령 할당자
ComPtr<ID3D12GraphicsCommandList> mCommandList; // 명령 목록
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
#pragma region 장치 생성

#if defined(DEBUG) || defined(_DEBUG)
	{ // 디버그층 활성화
		ComPtr<ID3D12Debug> debugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
		debugController->EnableDebugLayer();
	}
#endif


	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&mdxgiFactory)));

	// 하드웨어 어댑터를 나타내는 장치를 생성
	HRESULT hardwareResult = D3D12CreateDevice(
		nullptr,
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&md3dDevice));


	// 실패했다면 WARP 어댑터를 나타내는 장치를 생성
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
	   
#pragma region 4X MSAA 품질 수준 지원 점검
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

#pragma region 명령 대기열과 명령 목록 생성


	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	// CommandQueue 명령 대기열
	ThrowIfFailed(md3dDevice->CreateCommandQueue(
		&queueDesc, IID_PPV_ARGS(&mCommandQueue)));

	// CommandAllocator 명령 할당자
	ThrowIfFailed(md3dDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(mDirectCmdListAlloc.GetAddressOf())));

	// CommandList 명령 목록
	ThrowIfFailed(md3dDevice->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		mDirectCmdListAlloc.Get(),
		nullptr, // 그리기 명령 제출할게 없으므로 nullptr
		IID_PPV_ARGS(mCommandList.GetAddressOf())));

	// 명령 목록을 처음 호출할 때 Resetd을 호출하는데
	// Reset을 호출하려면 명령 목록이 닫혀있어야 함
	mCommandList->Close();
	

#pragma endregion

#pragma region  교환 사슬의 서술과 생성

	mSwapChain.Reset();

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = mClientWidth;
	sd.BufferDesc.Height = mClientHeight;
	sd.BufferDesc.RefreshRate.Denominator = 1; // 1분에
	sd.BufferDesc.RefreshRate.Numerator = 60; // 60번
	sd.BufferDesc.Format = mBackBufferFormat;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // 픽셀 순서 상관없이 출력
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // 스케일링 지정X (확장 배율, 디스플레이 중앙에 위치하게 하는등의 방법도 있음)
	sd.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	sd.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	sd.Windowed = true; // 창모드
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	//이 플래그를 사용하여 비트 블록 전송 모델을 지정IDXGISwapChain1::P resent1을 호출 후 DXGI가 백 버퍼의 콘텐츠를 삭제하도록 지정
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	// 창모드에서 정체 화면 모드로 전환하면 응용 프로그램의 창 크기랑 일치하게 디스플레이 모드가 변경됨

	ThrowIfFailed(mdxgiFactory->CreateSwapChain(
		mCommandQueue.Get(), // 명령 대기열의 포인터를 반환
		&sd,
		mSwapChain.GetAddressOf() // 포인터의 주소값을 반환
	));

	
#pragma endregion

#pragma region 서술자 힙 생성

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = SwapChainBufferCount; //  Heap desc 수
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;

	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
		&rtvHeapDesc, IID_PPV_ARGS(mRtvHeap.GetAddressOf())
	));

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	rtvHeapDesc.NumDescriptors = 1; //  Heap desc 수
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;

	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
		&dsvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())
	));
	
#pragma endregion

#pragma region 렌더 대상 뷰(RTV) 생성
	
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(
		mRtvHeap->GetCPUDescriptorHandleForHeapStart());
	
	for (UINT i = 0; i < SwapChainBufferCount; i++)
	{
		// 교환 사실의 i번째 버퍼를 얻는다.!
		ThrowIfFailed(mSwapChain->GetBuffer(
			i, IID_PPV_ARGS(&mSwapChainBuffer[i]) ));

		// 그 버퍼에 대한 RTV를 생성한다.!
		md3dDevice->CreateRenderTargetView(
			mSwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);

		// 힙의 다음 항목으로 넘어간다.
		rtvHeapHandle.Offset(1, mRtvDescriptorSize);
	}
#pragma endregion

#pragma region 깊이, 스텐실 버퍼와 뷰 생성
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
		&properties, // 기본 힙 GPU가 접근할 자원이 담김
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&optClear,
		IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf())
	));

	// 전체 자원이 밉맵 수준 0에 대한 서술자를
	// 해당 자원의 픽셀 형식을 적용해서 생성한다.
	md3dDevice->CreateDepthStencilView(
		mDepthStencilBuffer.Get(),
		nullptr,
		DepthStencilView()
	);



	// 자원을 초기 상태에서 깊이 버퍼로 사용할 수 있는 상태로 전이한다.
	mCommandList->ResourceBarrier(
		1,
		&resourceBarrier
	);



#pragma endregion

#pragma region 뷰포트 설정
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.Width = static_cast<float>(mClientWidth);
	vp.Height = static_cast<float>(mClientHeight);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	mCommandList->RSSetViewports(1, &vp);
#pragma endregion

#pragma region  가위 직사각형 설정
	mScissorRect = { 0, 0, mClientWidth / 2, mClientHeight / 2 };
	mCommandList->RSSetScissorRects(1, &mScissorRect);
#pragma endregion

#pragma region 성능 타이머
	// 정확한 시간 측정을 위해 사용

	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		__int64 countsPerSec;
		QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);

		double mSecondsPerCount = 1.0 / (double)countsPerSec;
	}

	// currTime 을 A라고 하고 그 다음에 QueryPerformanceCounter함수로 얻게되는 값을 B라고 할때
	// 어떤 작업에 걸린 시간은 (B - A) * mSecondsPerCount 초이다

#pragma endregion

#pragma region GameTimer 클래스
	// main.h에 기술함
#pragma endregion

#pragma region	프레임 간 경과 시간
	// 시간이 얼마나 흘렀는지 알아야함!
	// x가 n번째 프레임을 랜더링할때 측정한 성능 타이머 값이고
	// y가 그 이전 프레임에서의 성능 타이머 값이라고 하면
	// x  - y가 두 프레임 사이의 경과 시간이라고 할 수 있음
	
	// main.h의 GameTimer클래스에 기술 TICK 함수
	
	// Tick함수 호출
	// ----- main.h의 Run함수
	// ----- GameTimer의 Reset함수
#pragma endregion

#pragma region 전체 시간
	// 응용 프로그램이 시작된 이후에 흐른 시간을 뜻함 (정지된 시간 제외)
	// 전체 시간을 구현하기 위해 GameTime 클래스는 다음과 같은 멤버 변수를 사용하
	// mBaseTime, mPausedTime, mStopTime

	// GameTimer 클래스에 설명

	// GameTimer의 인스턴스를 추갈 생성하여 범용 '스톱워치'로 사용하는 것도 얼만든지 가능함
	// 예를 들어 폭탄의 도화선에 불이 붙었을 때 새 GameTimer인스턴스를 생성하고 TotalTime이 5초가 넘으면 폭탄이 폭발하는 시건을 촉발하는 등으로 활용할 수 있을 것이다.
#pragma endregion

#pragma region d3dApp
	HINSTANCE mhAppInst = nullptr; // 응용 프로그램 핸들
	HWND      mhMainWnd = nullptr; // 주 창 핸들
	bool      mAppPaused = false;  // 일시 정지된 상태인가?
	bool      mMinimized = false;  // 최소화된 상태인가?
	bool      mMaximized = false;  // 최대화된 상태인가?
	bool      mResizing = false;   // 크기 조정용 테두리를 끌고 있는 상태인가?
	bool      mFullscreenState = false;// 전체화면 활성화 여부

	// Set true to use 4X MSAA (?.1.8).  The default is false.
	bool      m4xMsaaState = false;    // 4X MSAA enabled
	UINT      m4xMsaaQuality = 0;      // quality level of 4X MSAA 
#pragma endregion
}
