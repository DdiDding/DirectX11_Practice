#pragma once
#include "D3DClass.h"
#include "CameraClass.h"
#include "ModelClass.h"
#include "ColorShaderClass.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.3f;

/* ApplicationClass
 * 앱의 모든 그래픽 기능을 캡슐화 하여 해당 클래스에서 관리하는 클래스
 */
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
	bool Render();
private:
	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	ModelClass* m_Model;
	ColorShaderClass* m_ColorShader;
};