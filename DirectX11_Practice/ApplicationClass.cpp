#include "ApplicationClass.h"
// ���� �Է� �� ��� ���� ���̺귯�� include
#include <fstream>
// ���� ����� "<<" �����ڸ� ����Ұ��̱⿡ include
#include <iostream>


ApplicationClass::ApplicationClass()
{
	m_Direct3D = nullptr;
	m_Camera = nullptr;
	m_Model = nullptr;
	m_ColorShader = nullptr;
	m_TextureShader = nullptr;
	m_LightShader = nullptr;
	m_Light = nullptr;
	m_Input = nullptr;
}


bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;
	char textureFileName[128];
	char modelFileName[128];

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


	// Create the camera object.
	m_Camera = new CameraClass;

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 90.0f, -500.0f);

	/////////////////////////////////////////////////////////
	// �� �ʱ�ȭ
	m_Model = new ModelClass;
	// ���� �̸� ����
	strcpy_s(modelFileName, "../Models/AnimMan.fbx");
	m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFileName, textureFileName);

	/*
	* //strcpy_s(modelFileName, "../Models/cube.txt");
	
	strcpy_s(textureFileName, "../Textures/stone01.tga");
	result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), modelFileName, textureFileName);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}*/

	// Color shader ����
	{
		m_ColorShader = new ColorShaderClass;
		result = m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd);
		if (!result)
		{
			MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
			return false;
		}
	}


	// Light shader ��ü�� �����ϰ� �ʱ�ȭ �մϴ�.
	{
		//m_LightShader = new LightShaderClass;

		//result = m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd);
		//if (!result)
		//{
		//	MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		//	return false;
		//}

		//// Light ��ü�� �����ϰ� �ʱ�ȭ �մϴ�.
		//m_Light = new LightClass;

		//m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
		//m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	}

	m_Input = new InputClass;
	m_Input->Initialize();

	return true;
}


void ApplicationClass::Shutdown()
{
	// Release the texture shader object.
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// Release the color shader object.
	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = nullptr;
	}

	// Release the model object.
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = nullptr;
	}

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = nullptr;
	}

	// Direct3D ��ü�� �����մϴ�. 
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = nullptr;
	}

	return;
}


bool ApplicationClass::Frame()
{
	static float rotation = 0.0f;
	bool result;

	// ȸ�� ���� �����Ӹ��� ������Ʈ �մϴ�.
	rotation += 0.005f;
	if (360.f < rotation)
	{
		rotation = 0.0f;
	}

	// �׷��� ����� �������մϴ�. 
	result = Render(rotation);

	if (result == false)
	{
		return false;
	}

	return true;
}


bool ApplicationClass::Render(float rotation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// ȭ���� ���������� �ʱ�ȭ
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶� ��ġ�� ������� �� ��� ����
	m_Camera->Render();

	// camera �� d3d objects���� world, view, projection ����� �����ɴϴ�.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	worldMatrix *= XMMatrixRotationRollPitchYaw(0, rotation, 0);

	// ������ ���������ο� ���� ����, �ε��� ���۸� ��ġ�Ͽ� �׸� �غ� �մϴ�.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	
	// �ؽ�ó ���̴��� ����Ͽ� ���� �������մϴ�.
	result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);

	/*result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(),m_Light->GetDirection(), m_Light->GetDiffuseColor());*/
	if (!result)
	{
		return false;
	}

	// �������� ����� ȭ�鿡 ǥ���Ѵ�.
	m_Direct3D->EndScene();

	return true;
}
