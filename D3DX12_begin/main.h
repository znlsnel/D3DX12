#pragma once

//#pragma comment(lib, "d3d12")
//#pragma comment(lib, "dxgi")

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")


#include <windows.h>
#include <wrl.h>
#include <d3dcommon.h>
#include <exception>
#include <d3d12sdklayers.h>
#include <dxgi1_4.h>
#include <assert.h>


#include "directx/d3dx12.h"


//#include <dxgi.h>
//#include <ppltasks.h>	// create_task의 경우
//#include <dxgidebug.h>


class GameTimer
{
public:
	GameTimer()
		: mSecondsPerCount(0.0), mDeltaTime(-1.0), mBaseTime(0), mStopTime(0),
		mPausedTime(0), mPrevTime(0), mCurrTime(0), mStopped(false)
	{
		__int64 countsPerSec;
		QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);

		mSecondsPerCount = 1.0 / (double)countsPerSec;
	}

	float TotalTime()const 
	{
		// 전체 시간을 반환
		if (mStopped)
		{
			return (float)(((mStopTime - mPausedTime) - mBaseTime) * mSecondsPerCount);
		}
		else
		{
			return (float)(((mCurrTime - mPausedTime) - mBaseTime) * mSecondsPerCount);
		}
	} // 초 단위

	float DeltaTime()const
	{
		return (float)mDeltaTime;
	} // 초 단위

	void Reset()
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		// 첫프레임에서 이전 프레임이라는 것이 없으므로 현재 시간을 넣어줌
		mPrevTime = currTime; 
		mBaseTime = currTime;
		mStopTime = 0;
		mStopped = false;
	} // 메시지 루프 이전에 호출

	void Start()
	{
		__int64 startTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

		if (mStopped)
		{
			// 일시정지된 시간의 길이 = 재개된 시점의 시간 - 정지된 시점의 시간
			mPausedTime += (startTime - mStopTime);

			mPrevTime = startTime; // Start이후 Tick에 쓰일 mPrevTime에 시작시간을 넣어줌
			mStopTime = 0;
			mStopped = false;
		}
	} // 타이머를 시작 또는 재개할 때 호출

	void Stop()
	{
		// 이미 정지 상태면 아무거또 안함
		if (!mStopped)
		{
			__int64 currTime;
			QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

			// 현재 시간을 정지 시점 시간으로 지정
			mStopTime = currTime; 
			mStopped = true;
		}
	} // 타이머를 정지할 때 호출

	void Tick()
	{
		if (mStopped)
		{
			mDeltaTime = 0.0;
			return;
		}

		// 이번 프레임의 시간을 얻음
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
		mCurrTime = currTime;

		// 작업 소요 시간 = (작업 수행 이후 현재 시간 - 작업 수행 전 시간) * mSecondsPerCount
		mDeltaTime = (mCurrTime - mPrevTime) * mSecondsPerCount;

		// 다음 프레임(Tick)을 위해 PrevTime 을 현재 시간으로 셋팅
		mPrevTime = mCurrTime;

		// 음수가 되지 않게 한다.
		// 프로세서가 절전 므로 들어가거나 실행이 다른 프로세서랑 엉키면
		// 간혹 음수가 된다고함
		if (mDeltaTime < 0.0)
		{
			mDeltaTime = 0.0;
		}
	}

private:
	double mSecondsPerCount;
	double mDeltaTime;

	__int64 mBaseTime; // 총 시간
	__int64 mPausedTime; // 일시정지 된 만큼의 시간
	__int64 mStopTime; // 정지된 시점의 시간
	__int64 mPrevTime; 
	__int64 mCurrTime; 

	bool mStopped;
};



inline void ThrowIfFailed(HRESULT hr)
{
	// DirectX API 오류를 탐지하기 위해 이 줄에 중단점 설정
	if (FAILED(hr))
		throw std::exception();
}




int mClientWidth = 800;
int mClientHeight = 800;
int mCurrBackBuffer = 0;

bool mAppPaused = false;
bool m4xMsaaState = false;
unsigned int m4xMsaaQuality = 0;
const int SwapChainBufferCount = 2;

GameTimer mTimer;

std::wstring mMainWndCaption = L"d3d App";
HWND      mhMainWnd = nullptr;

void CalculateFrameStats()
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.  These stats 
	// are appended to the window caption bar.

	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if ((mTimer.TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::wstring fpsStr = std::to_wstring(fps);
		std::wstring mspfStr = std::to_wstring(mspf);

		std::wstring windowText = mMainWndCaption +
			L"    fps: " + fpsStr +
			L"   mspf: " + mspfStr;

		SetWindowText(mhMainWnd, windowText.c_str());

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

void Update(const GameTimer& gt)
{

}

void Draw(const GameTimer& gt)
{

}

int Run()
{
	// MSG는 스레드 메시지 큐의 메시지 정보를 포함함
	MSG msg = { 0 };

	mTimer.Reset();

	// msg.message는 메시지 식별자 ( 종료 메시지가 아니면 반복 )
	while (msg.message != WM_QUIT)
	{
		// Windows 메시지가 있으면 처리함
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// 없으면 애니메이션 / 게임 작업을 수행함
		else
		{
			mTimer.Tick();

			if (!mAppPaused)
			{
				CalculateFrameStats();
				Update(mTimer);
				Draw(mTimer);
			}
			else
			{
				Sleep(100);
			}
		}
	}
	return (int)msg.wParam;
}