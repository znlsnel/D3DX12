#include "pch.h"
#include "Mesh.h"
#include "Engine.h"

void Mesh::Init(vector<Vertex>& vec)
{
	_vertexCount = static_cast<uint32>(vec.size());
	uint32 bufferSize = _vertexCount * sizeof(Vertex);

	// UPLOAD Type - CPU 쪽에서 작업을 한거를 GPU로 넘겨주는 용도로만 활용..
	// 보통은 UPLOAD, DEFAULT 두 개를 만들어서
	// 하나는  데이터를 전달하는 용도
	// 또 하나는 GPU에서 해당 buffer를 참조하는 용도로 사용
	// 하지만 지금은 UPLOAD를 공용으로 사용할거
	D3D12_HEAP_PROPERTIES heapProperty = 
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	// GPU쪽에 할당받을 공간의 크기를 buffer 사이즈만큼 할당 받음
	D3D12_RESOURCE_DESC desc = 
		CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	// GPU쪽에 가서 정보를 넣어주기 위해 공간을 할당받음
	DEVICE->CreateCommittedResource(
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_vertexBuffer));

	// 정점을 GPU 메모리에다 복사를 해줌
	// Copy the triangle data to the vertex buffer.
	void* vertexDataBuffer = nullptr;
	CD3DX12_RANGE readRange(0, 0); // We do not intend to read from this resource on the CPU.
	
	// Map이라는 함수를 이용해서 GPU메모리에 연결, Unmap으로 연결해제
	_vertexBuffer->Map(0, &readRange, &vertexDataBuffer);
	::memcpy(vertexDataBuffer, &vec[0], bufferSize); // 데이터 넣기
	_vertexBuffer->Unmap(0, nullptr);

	// Initialize the vertex buffer view.
	_vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
	_vertexBufferView.StrideInBytes = sizeof(Vertex); // 정점 1개 크기
	_vertexBufferView.SizeInBytes = bufferSize; // 버퍼의 크기	

}

void Mesh::Render()
{
	CMD_LIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CMD_LIST->IASetVertexBuffers(0, 1, &_vertexBufferView); // Slot: (0~15)

	// TODO
	// 1) Buffer에다가 데이터 세팅
	// 2) Buffer의 주소를 register에다가 전송
										// 0, 1은 b0, b1을 의미함
	GEngine->GetConstantBuffer()->PushData(0, &_transform, sizeof(_transform));
	GEngine->GetConstantBuffer()->PushData(1, &_transform, sizeof(_transform));

	// CMD_LIST->SetGraphicsRootConstantBufferView(0, ??)



	CMD_LIST->DrawInstanced(_vertexCount, 1, 0, 0);
}