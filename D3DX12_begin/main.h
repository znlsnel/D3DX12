#pragma once

#include <windows.h>

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// Windows ��� ����
#include <windows.h>
#include <SDKDDKVer.h>

// C ��Ÿ�� ��� �����Դϴ�.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>



// HWND - Handle WiNDow
// �� â�� �ڵ�, â �ڵ��� ������ â�� �ĺ��ϴ� �뵵�� ����
HWND ghMainWnd = 0; // gh - Get Handle

// Windows ���� ���α׷��� �ʱ�ȭ�� �ʿ��� �ڵ带 ���� �Լ�
// �ʱ�ȭ�� �����ϸ� true, �ƴϸ� false
// HINSTANCE  - Hangle + Instance
bool InitWindowsApp(HINSTANCE instanceHandle, int show);

// �޽��� ���� �ڵ带 ���� �Լ�
int Run();


// �� â�� ���� ��ǵ��� ó���ϴ� â ���ν��� �Լ�
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM IParam);

// windows ���� ���α׷��� �� ������. �ܼ� ���α׷��� main()�� �ش�




