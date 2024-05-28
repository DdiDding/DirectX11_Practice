#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include <fstream>
#include "TextureClass.h"
#include "EnumClass.h"

using namespace DirectX;
using namespace std;

class FbxTool;

class ModelClass
{
private:

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

public:
	ModelClass();
	ModelClass(const ModelClass&) = default;
	~ModelClass() = default;

	bool Initialize(ID3D11Device* , ID3D11DeviceContext* , char*, char*);
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

	bool LoadModel(char*);
	bool LoadFBX(char*);
	void ReleaseModel();

private:
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer * m_indexBuffer;
	int m_vertexCount;
	int m_indexCount;

	TextureClass* m_Texture;
	ModelType* m_model;

	VertexColor* m_vertices;
	unsigned int* m_indices;

	

	FbxTool* m_fbx;
};
