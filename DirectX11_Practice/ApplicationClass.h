#pragma once
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#include <windows.h>
#include "D3DClass.h"
#include "CameraClass.h"
#include "ModelClass.h"
#include "ColorShaderClass.h"
#include "TextureShaderClass.h"
#include "LightClass.h"
#include "LightShaderClass.h"
#include "InputClass.h"


const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.3f;

class ApplicationClass
{
public:
	ApplicationClass();
	ApplicationClass(const ApplicationClass&) = default;
	~ApplicationClass() = default;

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();

private:
	bool Render(float);

private:

	//  D3DClass의 포인터 추가, 이 튜토리얼에서 모든 멤버 변수는 m_접두사를 사용.
	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	ModelClass* m_Model;
	ColorShaderClass* m_ColorShader;
	TextureShaderClass* m_TextureShader;
	LightShaderClass* m_LightShader;
	LightClass* m_Light;

	InputClass* m_Input;
};