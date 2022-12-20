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

DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

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



}
