#include "ApplicationClass.h"
// 파일 입력 및 출력 위한 라이브러리 include
#include <fstream>
// 파일 쓰기시 "<<" 연산자를 사용할것이기에 include
#include <iostream>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "L2DFileDialog.h"

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

	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Platform/Renderer backends
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX11_Init(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext());
		ImGui::StyleColorsDark();
	}

	// Create the camera object.
	m_Camera = new CameraClass;

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 90.0f, -500.0f);

	/////////////////////////////////////////////////////////
	// 모델 초기화
	m_Model = new ModelClass;
	// 파일 이름 지정
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

	// Color shader 생성
	{
		m_ColorShader = new ColorShaderClass;
		result = m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd);
		if (!result)
		{
			MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
			return false;
		}
	}


	// Light shader 개체를 생성하고 초기화 합니다.
	{
		//m_LightShader = new LightShaderClass;

		//result = m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd);
		//if (!result)
		//{
		//	MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		//	return false;
		//}

		//// Light 개체를 생성하고 초기화 합니다.
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

	// Direct3D 개체를 해제합니다. 
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



	// 회전 값을 프레임마다 업데이트 합니다.
	rotation += 0.005f;
	if (360.f < rotation)
	{
		rotation = 0.0f;
	}

	// 그래픽 장면을 렌더링합니다. 
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

	// 화면을 검은색으로 초기화
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Imgui Render
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	
	// Open file browser
	{
		// L2DFileDialog code goes here.
		static char* file_dialog_buffer = new char[500];
		
		ImGui::Begin("L2DFileDialog Test");
		ImGui::Text("Path settings example");
		ImGui::Separator();

		// Choose a file
		ImGui::TextUnformatted("Choose a file");
		ImGui::SetNextItemWidth(380);
		ImGui::InputText("##path3", m_fbxPath, sizeof(m_fbxPath));
		ImGui::SameLine();
		FileDialog::file_dialog_choose = false;
		if (ImGui::Button("Browse##path3")) {
			file_dialog_buffer = m_fbxPath;
			FileDialog::file_dialog_open = true;
			FileDialog::file_dialog_open_type = FileDialog::FileDialogType::OpenFile;
		}

		if (FileDialog::file_dialog_open) {
			FileDialog::ShowFileDialog(&FileDialog::file_dialog_open, file_dialog_buffer, sizeof(file_dialog_buffer), FileDialog::file_dialog_open_type);
		}

		if (FileDialog::file_dialog_choose == true)
		{
			m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), m_fbxPath, m_fbxPath);
		}

		ImGui::End();
	}


	
	// 카메라 위치를 기반으로 뷰 행렬 생성
	m_Camera->Render();

	// camera 와 d3d objects에서 world, view, projection 행렬을 가져옵니다.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	worldMatrix *= XMMatrixRotationRollPitchYaw(0, rotation, 0);

	// 렌더링 파이프라인에 모델의 정점, 인덱스 버퍼를 배치하여 그릴 준비를 합니다.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	
	// 텍스처 셰이더를 사용하여 모델을 렌더링합니다.
	result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);

	/*result = m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture(),m_Light->GetDirection(), m_Light->GetDiffuseColor());
	if (!result)
	{
		return false;
	}*/

	// End Rendering
	// (Your code clears your framebuffer, renders your other stuff etc.)
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// 렌더링된 장면을 화면에 표시한다.
	m_Direct3D->EndScene();

	return true;
}
