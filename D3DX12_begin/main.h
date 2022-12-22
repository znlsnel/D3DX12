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
//#include <ppltasks.h>	// create_task�� ���
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
		// ��ü �ð��� ��ȯ
		if (mStopped)
		{
			return (float)(((mStopTime - mPausedTime) - mBaseTime) * mSecondsPerCount);
		}
		else
		{
			return (float)(((mCurrTime - mPausedTime) - mBaseTime) * mSecondsPerCount);
		}
	} // �� ����

	float DeltaTime()const
	{
		return (float)mDeltaTime;
	} // �� ����

	void Reset()
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		// ù�����ӿ��� ���� �������̶�� ���� �����Ƿ� ���� �ð��� �־���
		mPrevTime = currTime; 
		mBaseTime = currTime;
		mStopTime = 0;
		mStopped = false;
	} // �޽��� ���� ������ ȣ��

	void Start()
	{
		__int64 startTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

		if (mStopped)
		{
			// �Ͻ������� �ð��� ���� = �簳�� ������ �ð� - ������ ������ �ð�
			mPausedTime += (startTime - mStopTime);

			mPrevTime = startTime; // Start���� Tick�� ���� mPrevTime�� ���۽ð��� �־���
			mStopTime = 0;
			mStopped = false;
		}
	} // Ÿ�̸Ӹ� ���� �Ǵ� �簳�� �� ȣ��

	void Stop()
	{
		// �̹� ���� ���¸� �ƹ��Ŷ� ����
		if (!mStopped)
		{
			__int64 currTime;
			QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

			// ���� �ð��� ���� ���� �ð����� ����
			mStopTime = currTime; 
			mStopped = true;
		}
	} // Ÿ�̸Ӹ� ������ �� ȣ��

	void Tick()
	{
		if (mStopped)
		{
			mDeltaTime = 0.0;
			return;
		}

		// �̹� �������� �ð��� ����
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
		mCurrTime = currTime;

		// �۾� �ҿ� �ð� = (�۾� ���� ���� ���� �ð� - �۾� ���� �� �ð�) * mSecondsPerCount
		mDeltaTime = (mCurrTime - mPrevTime) * mSecondsPerCount;

		// ���� ������(Tick)�� ���� PrevTime �� ���� �ð����� ����
		mPrevTime = mCurrTime;

		// ������ ���� �ʰ� �Ѵ�.
		// ���μ����� ���� �Ƿ� ���ų� ������ �ٸ� ���μ����� ��Ű��
		// ��Ȥ ������ �ȴٰ���
		if (mDeltaTime < 0.0)
		{
			mDeltaTime = 0.0;
		}
	}

private:
	double mSecondsPerCount;
	double mDeltaTime;

	__int64 mBaseTime; // �� �ð�
	__int64 mPausedTime; // �Ͻ����� �� ��ŭ�� �ð�
	__int64 mStopTime; // ������ ������ �ð�
	__int64 mPrevTime; 
	__int64 mCurrTime; 

	bool mStopped;
};



inline void ThrowIfFailed(HRESULT hr)
{
	// DirectX API ������ Ž���ϱ� ���� �� �ٿ� �ߴ��� ����
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
	// MSG�� ������ �޽��� ť�� �޽��� ������ ������
	MSG msg = { 0 };

	mTimer.Reset();

	// msg.message�� �޽��� �ĺ��� ( ���� �޽����� �ƴϸ� �ݺ� )
	while (msg.message != WM_QUIT)
	{
		// Windows �޽����� ������ ó����
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// ������ �ִϸ��̼� / ���� �۾��� ������
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