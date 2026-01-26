#include "CameraClass.h"

CameraClass::CameraClass()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;
}

void CameraClass::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
	return;
}

void CameraClass::SetRocaiotn(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
	return;
}

XMFLOAT3 CameraClass::GetPosition()
{
	return XMFLOAT3(m_positionX, m_positionY, m_positionZ);
}

XMFLOAT3 CameraClass::GetRotation()
{
	return XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ);
}

void CameraClass::Render()
{
	//카메라 기본 방향 벡터들을 정의합니다.
	XMFLOAT3 up, position, lookAt;
	// Load it into a XMVECTOR structure.
	XMVECTOR upVector, positionVector, lookAtVector;
	{
		// 위를 바라보는 벡터 up을 정의합니다.
		up.x = 0.0f;
		up.y = 1.0f;
		up.z = 0.0f;
		upVector = XMLoadFloat3(&up);

		// 카메라의 위치를 position을 정의합니다.
		position.x = m_positionX;
		position.y = m_positionY;
		position.z = m_positionZ;
		positionVector = XMLoadFloat3(&position);

		// 카메라가 바라보는 방향 lookAt을 정의합니다.
		lookAt.x = 0.0f;
		lookAt.y = 0.0f;
		lookAt.z = 1.0f;
		lookAtVector = XMLoadFloat3(&lookAt);
	}

	// 회전 행렬 생성
	XMMATRIX rotationMatrix;
	{
		float yaw, pitch, roll;
		// 각도를 degree에서 radian으로 단위 변경
		pitch = m_rotationX * 0.0174532925f;
		yaw = m_rotationY * 0.0174532925f;
		roll = m_rotationZ * 0.0174532925f;

		// XMMatrixRotationRollPitchYaw로 회전 행렬 생성
		rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
	}

	// 회전 행렬을 이용하여 각 벡터 회전 적용
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// Finally create the view matrix from the three updated vectors.
	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

	return;
}

void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}
