#include "main.h"
using namespace Microsoft::WRL;

// Rtv : Render Target view
// Dsv : depth , stencil view 
// Cbv : Constant buffer view 
// Srv : Shader resource view
// Uav : Unordered access view (���� ���� ���� )

// stencill buffer�� ?
// Ư�� �ȼ����� �ĸ���ۿ� ��ϵ��� �ʵ��� �ϴ� ����
// ex) �׸���, �ſ� �������� ����


int mRtvDescriptorSize;
int mDsvDescriptorSize;
int mCbvSrvDescriptorSize;
unsigned int m4xMsaaQuality = 0;

ComPtr<ID3D12Device> md3dDevice;// = ComPtr<ID3D12Device>();
ComPtr<IDXGIFactory4> mdxgiFactory;
ComPtr<ID3D12Fence1> mFence;

ComPtr<ID3D12CommandQueue> mCommandQueue; // ��� ��⿭
ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc; // ��� �Ҵ���
ComPtr<ID3D12GraphicsCommandList> mCommandList; // ��� ���

#pragma region ��� ��⿭�� ��� ��� ����

void CreateCommandObjects()
{
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
}

#pragma endregion


int main() 
{

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



}
