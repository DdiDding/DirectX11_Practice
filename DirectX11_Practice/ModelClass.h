#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include "TextureClass.h"
using namespace DirectX;

class ModelClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};


public:
	ModelClass();
	ModelClass(const ModelClass&) = default;
	~ModelClass() = default;

	bool Initialize(ID3D11Device* , ID3D11DeviceContext* , char* );
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
	void ReleaseTexture();


private:
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer * m_indexBuffer;
	int m_vertexCount;
	int m_indexCount;

	TextureClass* m_Texture;
};
