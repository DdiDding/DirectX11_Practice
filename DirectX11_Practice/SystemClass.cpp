#include "SystemClass.h"


SystemClass::SystemClass()
{
	// 우리가 만든 클래스의 개체
	m_Input = nullptr;
	m_Application = nullptr;
}

bool SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	screenWidth = 0;
	screenHeight = 0;

	// windows api 초기화.
	InitializeWindows(screenWidth, screenHeight);

	// m_Input은 사용자의 키보드 입력을 처리하는데 사용된다.
	m_Input = new InputClass;
	m_Input->Initialize();

	// m_Application은 모든 그래픽 렌더링을 처리한다.
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
	// 클래스 개체들을 메모리 해제.
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

	// 창 종료
	ShutdownWindows();

	return;
}


// 애플리케이션의 처리는 Frame 함수에서 수행된다.
void SystemClass::Run()
{
	MSG msg;
	bool done, result;


	// 메세지 구조 초기화
	ZeroMemory(&msg, sizeof(MSG));

	// 창이나 사용자로부터 종료 메세지가 나타날 때까지 반복한다.
	// done이 true면 앱의 종료라는 뜻이다.
	done = false;
	while (!done)
	{
		// 윈도우 메세지의 처리
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// Window가 앱을 종료하라는 신호를 보내면 종료한다.
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// 종료 신호가 아니라면 프레임 처리를 수행한다.
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

	// 사용자가 ESC를 누르고 앱을 종료하려고하는지 확인.
	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	// Application 개체를 이용하여 프레임의 그래픽 작업을 수행한다.
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
		// 키보드에서의 키가 눌렸는지 확인.
		case WM_KEYDOWN:
		{
			// 키를 누르면 해당 상태를 저장할 수 있도록 Input개체에 전송한다.
			m_Input->KeyDown((unsigned int)wparam);
			return 0;
		}

		// 키보드에서 키가 해제되었는지 확인.
		case WM_KEYUP:
		{
			m_Input->KeyUp((unsigned int)wparam);
			return 0;
		}

		// 앱에서 사용하지 않는 다른 메세지는 기본 WinAPI메세지 핸들러로 전송한다.
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


	// System클래스의 포인터를 가져온다.
	ApplicationHandle = this;

	// HInstance는 앱(프로그램)의 핸들을 말한다. 운영체제가 제공해주는 핸들을 GetModuleHandle을 통해 가져온다.
	m_hinstance = GetModuleHandle(NULL);

	// 앱의 이름을 작성.
	m_applicationName = L"Engine";

	// 윈도우 클래스의 변수들을 작성한다. (앱의 창 크기, 앱의 핸들, ...)
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	// 윈도우 프로시저의 함수명을 지정.
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

	// 위에서 작성한 설정 값을 등록한다.
	RegisterClassEx(&wc);

	// 사용자의 모니터 해상도를 가져온다.
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// 전체 화면으로 실행하는지, 창 모드로 실행하는지에 따라 화면 설정을 설정합니다. 
	if (FULL_SCREEN)
	{
		// 전체 화면인 경우 화면을 사용자 데스크톱의 최대 크기 및 32비트로 설정합니다. 
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// 디스플레이 세팅을 전체 화면으로 변경한다.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// 창의 위치를 왼쪽 상단으로 설정한다.
		posX = posY = 0;
	}
	else
	{
		// 창 모드인 경우 해상도를 800x600으로 설정합니다. 
		screenWidth = 800;
		screenHeight = 600;

		// 창을 화면 중앙에 배치하게끔 왼쪽 상단 position을 구하여 설정한다.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// 화면 설정으로 창을 만들고 이에 대한 핸들을 가져온다.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// 창을 띄워 메인 포커스로 설정한다.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// 마우스 커서를 숨긴다.
	ShowCursor(false);

	return;
}

void SystemClass::ShutdownWindows()
{
	// Show the mouse cursor.
	ShowCursor(true);

	// 전체 화면을 종료하는 경우 디스플레이 설정을 수정한다.
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

		// 다른 메세지는 System 클래스의 MessageHandler 함수로 전달된다.
		// 이곳으로 메세지가 들어오는 이유느 Initialize 함수에서 wc.lpfnWndProc = WndProc으로 지정했기 때문.
		default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}
