#pragma once
#include "Object.h"

enum class RASTERIZER_TYPE
{
	CULL_NONE,		// 모두 다 연산
	CULL_FRONT,	// 시계 방향 무시
	CULL_BACK,		// 반시계 방향을 무시
	WIREFRAME, 
};

enum class DEPTH_STENCIL_TYPE
{
	LESS,				// 깊이값 < 1 일때만 그려줌
	LESS_EQUAL,			// 깊이값 <= 1 일때도 그려줌
	GREATER,			// 깊이값 > 1 
	GREATER_EQUAL,		// 깊이값 >= 1
};

struct ShaderInfo
{
	RASTERIZER_TYPE rasterizerType = RASTERIZER_TYPE::CULL_BACK;
	DEPTH_STENCIL_TYPE depthStencilType = DEPTH_STENCIL_TYPE::LESS;
};

// [일감 기술서] 외주 인력들이 뭘 해야할지 기술
class Shader : public Object
{
public:
	Shader();
	virtual ~Shader();

	void Init(const wstring& path, ShaderInfo info = ShaderInfo());
	void Update();

private:
	void CreateShader(const wstring& path, 
					 const string& name, 
					 const string& version, 
					 ComPtr<ID3DBlob>& blob, 
					 D3D12_SHADER_BYTECODE& shaderByteCode);

	void CreateVertexShader(const wstring& path, 
						     const string& name, 
						     const string& version);

	void CreatePixelShader(const wstring& path, 
						  const string& name, 
					  	  const string& version);


private:
	ComPtr<ID3DBlob>					_vsBlob;
	ComPtr<ID3DBlob>					_psBlob;
	ComPtr<ID3DBlob>					_errBlob;

	ComPtr<ID3D12PipelineState>			_pipelineState;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC  _pipelineDesc = {};

};

