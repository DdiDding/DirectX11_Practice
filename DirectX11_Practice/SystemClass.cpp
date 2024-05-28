#include "SystemClass.h"


SystemClass::SystemClass()
{
	// �츮�� ���� Ŭ������ ��ü
	m_Input = nullptr;
	m_Application = nullptr;
}

bool SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	screenWidth = 0;
	screenHeight = 0;

	// windows api �ʱ�ȭ.
	InitializeWindows(screenWidth, screenHeight);

	// m_Input�� ������� Ű���� �Է��� ó���ϴµ� ���ȴ�.
	m_Input = new InputClass;
	m_Input->Initialize();

	// m_Application�� ��� �׷��� �������� ó���Ѵ�.
	m_Application = new ApplicationClass;
	result = m_Application->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result)
	{
		return false;
	}

	

	return true;
}

void SystemClass::Shutdown()
{
	// Ŭ���� ��ü���� �޸� ����.
	{
		if (m_Application)
		{
			m_Application->Shutdown();
			delete m_Application;
			m_Application = 0;
		}

		if (m_Input)
		{
			delete m_Input;
			m_Input = 0;
		}
	}

	// â ����
	ShutdownWindows();

	return;
}


// ���ø����̼��� ó���� Frame �Լ����� ����ȴ�.
void SystemClass::Run()
{
	MSG msg;
	bool done, result;


	// �޼��� ���� �ʱ�ȭ
	ZeroMemory(&msg, sizeof(MSG));

	// â�̳� ����ڷκ��� ���� �޼����� ��Ÿ�� ������ �ݺ��Ѵ�.
	// done�� true�� ���� ������ ���̴�.
	done = false;
	while (!done)
	{
		// ������ �޼����� ó��
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// Window�� ���� �����϶�� ��ȣ�� ������ �����Ѵ�.
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// ���� ��ȣ�� �ƴ϶�� ������ ó���� �����Ѵ�.
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}

	}

	return;
}

bool SystemClass::Frame()
{
	bool result;

	// ����ڰ� ESC�� ������ ���� �����Ϸ����ϴ��� Ȯ��.
	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	// Application ��ü�� �̿��Ͽ� �������� �׷��� �۾��� �����Ѵ�.
	result = m_Application->Frame();
	if (!result)
	{
		return false;
	}

	return true;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		// Ű���忡���� Ű�� ���ȴ��� Ȯ��.
		case WM_KEYDOWN:
		{
			// Ű�� ������ �ش� ���¸� ������ �� �ֵ��� Input��ü�� �����Ѵ�.
			m_Input->KeyDown((unsigned int)wparam);
			return 0;
		}

		// Ű���忡�� Ű�� �����Ǿ����� Ȯ��.
		case WM_KEYUP:
		{
			m_Input->KeyUp((unsigned int)wparam);
			return 0;
		}

		// �ۿ��� ������� �ʴ� �ٸ� �޼����� �⺻ WinAPI�޼��� �ڵ鷯�� �����Ѵ�.
		default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;


	// SystemŬ������ �����͸� �����´�.
	ApplicationHandle = this;

	// HInstance�� ��(���α׷�)�� �ڵ��� ���Ѵ�. �ü���� �������ִ� �ڵ��� GetModuleHandle�� ���� �����´�.
	m_hinstance = GetModuleHandle(NULL);

	// ���� �̸��� �ۼ�.
	m_applicationName = L"Engine";

	// ������ Ŭ������ �������� �ۼ��Ѵ�. (���� â ũ��, ���� �ڵ�, ...)
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	// ������ ���ν����� �Լ����� ����.
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// ������ �ۼ��� ���� ���� ����Ѵ�.
	RegisterClassEx(&wc);

	// ������� ����� �ػ󵵸� �����´�.
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// ��ü ȭ������ �����ϴ���, â ���� �����ϴ����� ���� ȭ�� ������ �����մϴ�. 
	if (FULL_SCREEN)
	{
		// ��ü ȭ���� ��� ȭ���� ����� ����ũ���� �ִ� ũ�� �� 32��Ʈ�� �����մϴ�. 
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// ���÷��� ������ ��ü ȭ������ �����Ѵ�.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// â�� ��ġ�� ���� ������� �����Ѵ�.
		posX = posY = 0;
	}
	else
	{
		// â ����� ��� �ػ󵵸� 800x600���� �����մϴ�. 
		screenWidth = 800;
		screenHeight = 600;

		// â�� ȭ�� �߾ӿ� ��ġ�ϰԲ� ���� ��� position�� ���Ͽ� �����Ѵ�.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// ȭ�� �������� â�� ����� �̿� ���� �ڵ��� �����´�.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// â�� ��� ���� ��Ŀ���� �����Ѵ�.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// ���콺 Ŀ���� �����.
	ShowCursor(false);

	return;
}

void SystemClass::ShutdownWindows()
{
	// Show the mouse cursor.
	ShowCursor(true);

	// ��ü ȭ���� �����ϴ� ��� ���÷��� ������ �����Ѵ�.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window.
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// Remove the application instance.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// Release the pointer to this class.
	ApplicationHandle = NULL;

	return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// Check if the window is being destroyed.
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		// Check if the window is being closed.
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		// �ٸ� �޼����� System Ŭ������ MessageHandler �Լ��� ���޵ȴ�.
		// �̰����� �޼����� ������ ������ Initialize �Լ����� wc.lpfnWndProc = WndProc���� �����߱� ����.
		default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}
