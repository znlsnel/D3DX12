#pragma once

#include <windows.h>

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>
#include <SDKDDKVer.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>



// HWND - Handle WiNDow
// 주 창의 핸들, 창 핸들은 생성된 창을 식별하는 용도로 쓰임
HWND ghMainWnd = 0; // gh - Get Handle

// Windows 응용 프로그램의 초기화에 필요한 코드를 담은 함수
// 초기화에 성공하면 true, 아니면 false
// HINSTANCE  - Hangle + Instance
bool InitWindowsApp(HINSTANCE instanceHandle, int show);

// 메시지 루프 코드를 담은 함수
int Run();


// 주 창이 받은 사건들을 처리하는 창 프로시저 함수
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM IParam);

// windows 응용 프로그램의 주 진입점. 콘솔 프로그램의 main()에 해당




