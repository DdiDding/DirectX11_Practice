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


	// Direct3D ��ü�� ���� �� �ʱ�ȭ �մϴ�.
	m_Direct3D = new D3DClass;

	// �� Ŭ������ ���� ���� 4���� â�� ���� ���� ���̸� �Ű������� D3DClass�� �ʱ�ȭ �Լ��� ȣ���Ѵ�.
	// �׷��� D3DClass�� �� �Ű������� �̿��� Direct3D �ý����� �����Ѵ�.
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
	// Direct3D ��ü�� �����մϴ�. 
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


	// �׷��� ����� �������մϴ�. 
	result = Render();

	if (result == false)
	{
		return false;
	}

	return true;
}


bool ApplicationClass::Render()
{
	// Scene�� �����Ϸ��� ���۸� �����.
	m_Direct3D->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);

	// �������� ����� ȭ�鿡 ǥ���Ѵ�.
	m_Direct3D->EndScene();

	return true;
}
