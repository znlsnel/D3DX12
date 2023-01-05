#pragma once
class Texture;
class Material;

// [����Ƽ¯]�� ���� �������� �̷���� ��ü
class Mesh
{
public:
	void Init(const vector<Vertex>& vertexBuffer, const vector<uint32>& indextbuffer);
	void Render();


private:
	void CreateVertexBuffer(const vector<Vertex>& buffer);
	void CreateIndexBuffer(const vector<uint32>& buffer);

private:
	// VBV
	ComPtr<ID3D12Resource>			_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW		_vertexBufferView = {};
	uint32							_vertexCount = 0;

	// IBV
	ComPtr<ID3D12Resource>			_indexBuffer;
	D3D12_INDEX_BUFFER_VIEW		_indexBufferView;
	uint32 _indexCount = 0;

};


  