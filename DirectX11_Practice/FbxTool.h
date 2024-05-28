#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include <fbxsdk.h>
#include "EnumClass.h"

using namespace DirectX;
using namespace std;

class FbxTool
{
public:

	FbxTool();
	~FbxTool() = default;

	bool Initialize();

	bool Load(const char* fileName);
	
	XMFLOAT3* GetVertexPos() { return m_pos; };
	unsigned int* GetVertexIdx() { return m_idx; };


private:

	// ��ͷ� �޽� ��带 ã�� m_mesh�� �����մϴ�.
	bool FindMesh(FbxNode* node);

	bool SaveVertexData();

	void Shotdown();

private:

	FbxManager* m_manager;
	// FbxImporter�� FbxSene�� Fbx���Ͽ� ä��ϴ�.
	FbxImporter* m_importer;
	FbxScene* m_scene;
	FbxMesh* m_mesh;

	XMFLOAT3* m_pos;
	unsigned int* m_idx;
};
