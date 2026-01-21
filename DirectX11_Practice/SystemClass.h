#pragma once
#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_

// 암호화, DDE, RPC, 셸 및 Windows 소켓과 같은 API를 제외 하여 빌드 프로세스 속도를 향상
#define WIN32_LEAN_AND_MEAN

// win32 함수를 사용하려는 목적인 windows.h 포함
#include <windows.h>
#include "inputclass.h"
#include "applicationclass.h"

/**
 * @class SystemClass
 * WinMain에서 호출되는 Initialize, Shutdown, Run 함수가 여기에 정의된다.
 * Windows 시스템 메세지를 처리하기 위해 MessageHandler 함수도 포함된다.
 * 마지막으로 입력과 그래픽 렌더링을 처리할 두 객체를 가리키는 Private 포인터도 존재한다.
 */
class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	// 소멸자가 호출되지 않을 수도 있는 경우가 가끔 생기기때문에, Shutdown함수를 만들어 해당 함수에서 소멸작업을 처리한다. 그 이유로 소멸자에 아무것도 없으므로 default 사용한다.
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

	// LPCWSTR == const char*
	LPCWSTR m_applicationName;
	// 윈도우 운영체제에서 실행되는 프로그램들을 구별하기 위한 ID값을 가진 Handle
	HINSTANCE m_hinstance;
	// 하나의 프로그램에서 여러개의 창을 가질 수 있기 때문에, 각 창의 핸들이 존재한다.
	HWND m_hwnd;

	InputClass* m_Input;
	ApplicationClass* m_Application;
};

/* @function WndProc
 * 윈도우가 메세지를 보내는 곳.
 * InitializeWindows 함수에서 wc.lpfnWndProc = WndProc 와 같이 윈도우 클래스를 초기화할 때 윈도우에 이 함수의 이름을 알려주었다.
 * 이 함수가 있음에도 MessageHandler를 구현한 이유는 OS단의 메세지처리, App단의 메세지 처리를 구분하고 싶었기 때문이라 생각한다.
 * 또한 우리는 메세지 처리시 여러 객체에 접근해야하는데[ex) m_Input->Keydown()] WndProc만 존재한다면 전역 함수이기 떄문에 이런게 불가능해진다.
 */
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static SystemClass* ApplicationHandle = 0;

#endif

