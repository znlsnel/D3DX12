#pragma once

class ConstantBuffer
{
public:
	ConstantBuffer();
	~ConstantBuffer();

	void Init(uint32 size, uint32 count);
	void Clear();

	D3D12_CPU_DESCRIPTOR_HANDLE PushData(int32 rootParamIndex, void* buffer, uint32 size);
	D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress(uint32 index);
	D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(uint32 index);

private:
	void CreateBuffer();
	void CreateView();

private:
	ComPtr<ID3D12Resource>	_cbvBuffer; // 모든 buffer의 모음집
	BYTE*					_mappedBuffer = nullptr;
	uint32					_elementSize = 0;
	uint32					_elementCount = 0;
	
	// constant Buffer Heap
	ComPtr<ID3D12DescriptorHeap>			_cbvHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE			_cpuHandleBegin = {}; // 시작 핸들
	uint32									_handleIncrementSize = 0; // 핸들간의 간격

	uint32									_currentIndex = 0; // 내가 사용중인 위치


};

