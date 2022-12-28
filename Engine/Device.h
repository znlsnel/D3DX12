#pragma once

// 인력 사무소
class Device
{
public:
	void Init();

	ComPtr<IDXGIFactory> GetDXGI() { return _dxgi; }
	ComPtr<ID3D12Device> GetDevice() { return _device; }

private:
	
	// COM(Component Object Model)
	// - DX의 프로그래밍 언어 독립성과 하위 호환성을 가능하게 하는 기술
	// - COM 객체(COM 인터페이스)를 사용. 세부사항은 우리한테 숨겨짐
	// - ComPtr 일종의 스마트 포인터

	ComPtr<ID3D12Debug> _debugController;
	ComPtr<IDXGIFactory> _dxgi; // 화면 관련 기능들
	// device를 통해서 commandQueue, Alloc, list, SwapChain.... 등등 모든걸 만듬
	ComPtr<ID3D12Device> _device;

};

