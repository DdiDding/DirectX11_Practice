#pragma once
/********************************************************/
/* includes */
/********************************************************/
#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX;


/********************************************************/
/* class */
/********************************************************/
// 3D모델을 캡슐화하는 역할을 한다. -> 3D모델을 불러와 인터페이스만 제공하겠다는 의미
class ModelClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

public:
	ModelClass();
	ModelClass(const ModelClass&) = default;
	~ModelClass() = default;

	// 정점 버퍼와 인덱스 버퍼의 초기화 함수를 호출한다.
	bool Initialize(ID3D11Device*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	// RenderBuffers 함수는 Render 함수에서 호출됩니다. 이 함수의 목적은 GPU의 IA단계에서 정점 버퍼와 인덱스 버퍼를 활성 상태로 설정하는 것입니다. GPU가 활성 정점 버퍼를 가지게 되면 셰이더를 사용하여 해당 버퍼를 렌더링할 수 있습니다. 이 함수는 또한 삼각형, 선, 팬 등 해당 버퍼가 어떻게 그려져야 하는지도 정의합니다. 이 튜토리얼에서는 입력 어셈블러에서 버텍스 버퍼와 인덱스 버퍼를 활성 상태로 설정하고, IASetPrimitiveTopology DirectX 함수를 사용하여 GPU에 해당 버퍼를 삼각형으로 그려야 함을 지시합니다.
	void RenderBuffers(ID3D11DeviceContext* deviceContext);

private:
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	int m_vertexCount, m_indexCount;
};

