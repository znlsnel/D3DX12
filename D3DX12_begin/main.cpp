#include "main.h"
using namespace Microsoft::WRL;

// Rtv : Render Target view
// Dsv : depth , stencil view 
// Cbv : Constant buffer view 
// Srv : Shader resource view
// Uav : Unordered access view (순서 없는 접근 )

// stencill buffer란 ?
// 특정 픽셀들이 후면버퍼에 기록되지 않도록 하는 버퍼
// ex) 그림자, 거울 랜더링시 사용됨


int mRtvDescriptorSize;
int mDsvDescriptorSize;
int mCbvSrvDescriptorSize;
unsigned int m4xMsaaQuality = 0;

ComPtr<ID3D12Device> md3dDevice;// = ComPtr<ID3D12Device>();
ComPtr<IDXGIFactory4> mdxgiFactory;
ComPtr<ID3D12Fence1> mFence;

ComPtr<ID3D12CommandQueue> mCommandQueue; // 명령 대기열
ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc; // 명령 할당자
ComPtr<ID3D12GraphicsCommandList> mCommandList; // 명령 목록

#pragma region 명령 대기열과 명령 목록 생성

void CreateCommandObjects()
{
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
}

#pragma endregion


int main() 
{

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



}
