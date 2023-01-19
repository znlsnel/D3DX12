#include "pch.h"
#include "Shader.h"
#include "Engine.h"


Shader::Shader() : Object(OBJECT_TYPE::SHADER)
{

}

Shader::~Shader()
{
	 
}

void Shader::CreateGraphicsShader(const wstring& path, ShaderInfo info, const string& vs, const string& ps)
{
	_info = info;

	CreateVertexShader(path, vs, "vs_5_0");
	CreatePixelShader(path, ps, "ps_5_0");

	D3D12_INPUT_ELEMENT_DESC desc[] =
	{ 
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	__graphicsPipelineDesc.InputLayout = { desc, _countof(desc) };
	__graphicsPipelineDesc.pRootSignature = GRAPHICS_ROOT_SIGNATURE.Get();

	__graphicsPipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	__graphicsPipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	__graphicsPipelineDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	__graphicsPipelineDesc.SampleMask = UINT_MAX;
	__graphicsPipelineDesc.PrimitiveTopologyType = info.topologyType;
	__graphicsPipelineDesc.NumRenderTargets = 1;
	__graphicsPipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	__graphicsPipelineDesc.SampleDesc.Count = 1;
	__graphicsPipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	switch (info.shaderType)
	{
	case SHADER_TYPE::DEFERRED:
		__graphicsPipelineDesc.NumRenderTargets = RENDER_TARGET_G_BUFFER_GROUP_MEMBER_COUNT;
		__graphicsPipelineDesc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT; // POSITION
		__graphicsPipelineDesc.RTVFormats[1] = DXGI_FORMAT_R32G32B32A32_FLOAT; // NORMAL
		__graphicsPipelineDesc.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM; // COLOR
		break;

	case SHADER_TYPE::FORWARD:
		__graphicsPipelineDesc.NumRenderTargets = 1;
		__graphicsPipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		break;

	case SHADER_TYPE::LIGHTING:
		__graphicsPipelineDesc.NumRenderTargets = 2;
		__graphicsPipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		__graphicsPipelineDesc.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;
		break;
	}

	switch (info.rasterizerType)
	{
	case RASTERIZER_TYPE::CULL_BACK:
		__graphicsPipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		__graphicsPipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
		break;
	case RASTERIZER_TYPE::CULL_FRONT:
		__graphicsPipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		__graphicsPipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
		break;
	case RASTERIZER_TYPE::CULL_NONE:
		__graphicsPipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		__graphicsPipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		break;
	case RASTERIZER_TYPE::WIREFRAME:
		__graphicsPipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
		__graphicsPipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		break;
	}

	switch (info.depthStencilType)
	{
	case DEPTH_STENCIL_TYPE::LESS:
		__graphicsPipelineDesc.DepthStencilState.DepthEnable = TRUE;
		__graphicsPipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		break;
	case DEPTH_STENCIL_TYPE::LESS_EQUAL:
		__graphicsPipelineDesc.DepthStencilState.DepthEnable = TRUE;
		__graphicsPipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		break;
	case DEPTH_STENCIL_TYPE::GREATER:
		__graphicsPipelineDesc.DepthStencilState.DepthEnable = TRUE;
		__graphicsPipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_GREATER;
		break;
	case DEPTH_STENCIL_TYPE::GREATER_EQUAL:
		__graphicsPipelineDesc.DepthStencilState.DepthEnable = TRUE;
		__graphicsPipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		break;
	case DEPTH_STENCIL_TYPE::NO_DEPTH_TEST:
		__graphicsPipelineDesc.DepthStencilState.DepthEnable = FALSE;
		__graphicsPipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		break;
	case DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE:
		__graphicsPipelineDesc.DepthStencilState.DepthEnable = FALSE;
		__graphicsPipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		break;
	case DEPTH_STENCIL_TYPE::LESS_NO_WRITE:
		__graphicsPipelineDesc.DepthStencilState.DepthEnable = TRUE;
		__graphicsPipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		__graphicsPipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		break;
	}

	D3D12_RENDER_TARGET_BLEND_DESC& rt = __graphicsPipelineDesc.BlendState.RenderTarget[0];

	// SrcBlend = Pixel Shader
	// DestBlend = Render Target
	switch (info.blendType)
	{
	case BLEND_TYPE::DEFAULT:
		rt.BlendEnable = FALSE;
		rt.LogicOpEnable = FALSE;
		rt.SrcBlend = D3D12_BLEND_ONE;
		rt.DestBlend = D3D12_BLEND_ZERO;
		break;
	case BLEND_TYPE::ALPHA_BLEND:
		rt.BlendEnable = TRUE;
		rt.LogicOpEnable = FALSE;
		rt.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		rt.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		break;
	case BLEND_TYPE::ONE_TO_ONE_BLEND:
		rt.BlendEnable = TRUE;
		rt.LogicOpEnable = FALSE;
		rt.SrcBlend = D3D12_BLEND_ONE;
		rt.DestBlend = D3D12_BLEND_ONE;
		break;
	}

	DEVICE->CreateGraphicsPipelineState(&__graphicsPipelineDesc, IID_PPV_ARGS(&_pipelineState));
}

void Shader::CreateComputeShader(const wstring& path, const string& name, const string& version)
{
	_info.shaderType = SHADER_TYPE::COMPUTE;

	CreateShader(path, name, version, _csBlob, _computePipelineDesc.CS);
	_computePipelineDesc.pRootSignature = COMPUTE_ROOT_SIGNATURE.Get();

	HRESULT hr = DEVICE->CreateComputePipelineState(&_computePipelineDesc, IID_PPV_ARGS(&_pipelineState));
	assert(SUCCEEDED(hr));
}

void Shader::Update()
{
	if (GetShaderType() == SHADER_TYPE::COMPUTE)
		COMPUTE_CMD_LIST->SetPipelineState(_pipelineState.Get());
	else
		GRAPHICS_CMD_LIST->SetPipelineState(_pipelineState.Get());
}

void Shader::CreateShader(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob, D3D12_SHADER_BYTECODE& shaderByteCode)
{
	uint32 compileFlag = 0;
#ifdef _DEBUG
	compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	if (FAILED(::D3DCompileFromFile(path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, name.c_str(), version.c_str(), compileFlag, 0, &blob, &_errBlob)))
	{
		::MessageBoxA(nullptr, "Shader Create Failed !", nullptr, MB_OK);
	}

	shaderByteCode = { blob->GetBufferPointer(), blob->GetBufferSize() };
}

void Shader::CreateVertexShader(const wstring& path, const string& name, const string& version)
{
	CreateShader(path, name, version, _vsBlob, __graphicsPipelineDesc.VS);
}

void Shader::CreatePixelShader(const wstring& path, const string& name, const string& version)
{
	CreateShader(path, name, version, _psBlob, __graphicsPipelineDesc.PS);
}