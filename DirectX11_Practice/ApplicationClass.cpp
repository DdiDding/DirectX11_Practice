#include "ApplicationClass.h"

ApplicationClass::ApplicationClass()
{
	m_Direct3D = nullptr;
}


ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}


bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;


	// Direct3D 개체를 생성 및 초기화 합니다.
	m_Direct3D = new D3DClass;

	// 이 클래스의 전역 변수 4개와 창의 가로 세로 길이를 매개변수로 D3DClass의 초기화 함수를 호출한다.
	// 그러면 D3DClass는 이 매개변수를 이용해 Direct3D 시스템을 설정한다.
	result = m_Direct3D->Initialize( screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	return true;
}


void ApplicationClass::Shutdown()
{
	// Direct3D 개체를 해제합니다. 
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	return;
}


bool ApplicationClass::Frame()
{
	bool result;


	// 그래픽 장면을 렌더링합니다. 
	result = Render();

	if (result == false)
	{
		return false;
	}

	return true;
}


bool ApplicationClass::Render()
{
	// Scene을 시작하려면 버퍼를 지운다.
	m_Direct3D->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);

	// 렌더링된 장면을 화면에 표시한다.
	m_Direct3D->EndScene();

	return true;
}
