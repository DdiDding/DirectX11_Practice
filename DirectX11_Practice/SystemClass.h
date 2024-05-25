#pragma once

// ��ȣȭ, DDE, RPC, �� �� Windows ���ϰ� ���� API�� ���� �Ͽ� ���� ���μ��� �ӵ��� ���
#define WIN32_LEAN_AND_MEAN

// win32 �Լ��� ����Ϸ��� ������ windows.h ����
#include <windows.h>
#include "inputclass.h"
#include "applicationclass.h"

class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	// �Ҹ��ڰ� ȣ����� ���� ���� �ִ� ��찡 �ֱ� ������ �Ҹ� �۾��� Shutdown�Լ��� ����.
	// �׷��� �⺻ �Ҹ��ڿ� �ƹ��͵� �����Ƿ� default ���
	~SystemClass() = default;

	bool Initialize();
	void Shutdown();
	void Run();

	// Windows �ý��� �޽����� ó���ϱ� ���� �ڵ�
	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	// ���������� �� �����Ӹ��� ���� ó���� ���
	bool Frame();
	// ���� �ʱ�ȭ �۾�
	void InitializeWindows(int&, int&);
	// ���� ����� �۾�
	void ShutdownWindows();

private:

	// LPCWSTR = const char*
	LPCWSTR m_applicationName;
	// ������ �ü������ ����Ǵ� ���α׷����� �����ϱ� ���� ID���� �ǹ��ϴ� ���� Handle
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	// �츮�� ���� Ŭ����
	InputClass* m_Input;
	ApplicationClass* m_Application;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static SystemClass* ApplicationHandle = 0;

