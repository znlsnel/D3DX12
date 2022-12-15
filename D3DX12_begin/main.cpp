#include "main.h"
using namespace Microsoft::WRL;



int main() 
{

	#if defined(DEBUG) || defined(_DEBUG)
	{ // ������� Ȱ��ȭ
		ComPtr<ID3D12Debug> debugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
		debugController->EnableDebugLayer();
	}
	#endif


	ComPtr<IDXGIFactory4> mdxgiFactory;
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&mdxgiFactory)));


	// �ϵ���� ����͸� ��Ÿ���� ��ġ�� ����
	ComPtr<ID3D12Device> md3dDevice;// = ComPtr<ID3D12Device>();
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
}
