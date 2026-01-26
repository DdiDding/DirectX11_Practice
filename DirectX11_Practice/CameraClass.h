#pragma once
//HLSL 셰이더 코딩 방법, 버텍스 및 인덱스 버퍼 설정 방법, ColorShaderClass를 사용해 해당 버퍼를 렌더링하기 위한 HLSL 셰이더 호출 방법을 살펴보았습니다. 그러나 아직 부족한 한 가지는 렌더링을 위한 시점입니다. 이를 위해 DirectX 11이 장면을 어디서, 어떻게 바라보는지 알 수 있도록 카메라 클래스가 필요합니다. 카메라 클래스는 카메라의 위치와 현재 회전 상태를 추적합니다. 이 위치 및 회전 정보를 활용해 뷰 매트릭스를 생성하며, 이 매트릭스는 렌더링을 위해 HLSL 셰이더로 전달됩니다.
/********************************************************/
/* includes */
/********************************************************/
#include <directxmath.h>
using namespace DirectX;

/********************************************************/
/* Main */
/********************************************************/
class CameraClass
{
public:
	CameraClass();
	CameraClass(const CameraClass&) = default;
	~CameraClass() = default;

	void SetPosition(float x, float y, float z);
	void SetRocaiotn(float x, float y, float z);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	void Render();
	void GetViewMatrix(XMMATRIX& viewMatrix);

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	XMMATRIX m_viewMatrix;
};

