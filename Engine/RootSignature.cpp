#include "pch.h"
#include "RootSignature.h"
#include "Engine.h"
void RootSignature::Init(ComPtr<ID3D12Device> device)
{ 
	CD3DX12_DESCRIPTOR_RANGE ranges[] =
	{
		CD3DX12_DESCRIPTOR_RANGE(
			D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 
			CBV_REGISTER_COUNT, // 갯수 ( 여기서는 5개로 함 )
			0), // b0~b4
	};


	// param에 각각의 view를 만드는 것이 아니라
	// 여러개의 view가 들어있는 테이블을 만들어줌
	CD3DX12_ROOT_PARAMETER param[1];
	param[0].InitAsDescriptorTable(_countof(ranges), ranges); 

	

	// param을 활용
	D3D12_ROOT_SIGNATURE_DESC sigDesc = 
		CD3DX12_ROOT_SIGNATURE_DESC(_countof(param), param);

	sigDesc.Flags = // 입력 조립기 단계를 허용
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT; 

	ComPtr<ID3DBlob> blobSignature;
	ComPtr<ID3DBlob> blobError;

	::D3D12SerializeRootSignature(&sigDesc, 
								D3D_ROOT_SIGNATURE_VERSION_1, 
								&blobSignature, 
								&blobError);

	device->CreateRootSignature(0, 
								blobSignature->GetBufferPointer(), 
								blobSignature->GetBufferSize(), 
								IID_PPV_ARGS(&_signature));
}