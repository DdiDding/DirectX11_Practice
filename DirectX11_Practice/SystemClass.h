#pragma once

// 암호화, DDE, RPC, 셸 및 Windows 소켓과 같은 API를 제외 하여 빌드 프로세스 속도를 향상
#define WIN32_LEAN_AND_MEAN

// win32 함수를 사용하려는 목적인 windows.h 포함
#include <windows.h>
#include "inputclass.h"
#include "applicationclass.h"

class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	// 소멸자가 호출되지 않을 수도 있는 경우가 있기 때문에 소멸 작업은 Shutdown함수에 구현.
	// 그래서 기본 소멸자에 아무것도 없으므로 default 사용
	~SystemClass() = default;

	bool Initialize();
	void Shutdown();
	void Run();

	// Windows 시스템 메시지를 처리하기 위한 핸들
	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	// 실질적으로 매 프레임마다 앱의 처리를 담당
	bool Frame();
	// 앱의 초기화 작업
	void InitializeWindows(int&, int&);
	// 앱의 종료시 작업
	void ShutdownWindows();

private:

	// LPCWSTR = const char*
	LPCWSTR m_applicationName;
	// 윈도우 운영체제에서 실행되는 프로그램들을 구별하기 위한 ID값을 의미하는 앱의 Handle
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	// 우리가 만든 클래스
	InputClass* m_Input;
	ApplicationClass* m_Application;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static SystemClass* ApplicationHandle = 0;

