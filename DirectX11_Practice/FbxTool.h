#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include <fbxsdk.h>
using namespace DirectX;
using namespace std;

class FbxTool
{
public:

	FbxTool();
	~FbxTool() = default;

	bool Initialize();

	bool Load(const char* fileName, XMFLOAT3** vertexPos, int** vertexIdx, int& vertexCnt);
	
	// ��ͷ� �޽� ��带 ã�� m_mesh�� �����մϴ�.
	bool FindMesh(FbxNode* node);

	bool SaveVertexData(XMFLOAT3** vertexPos, int** vertexIdx, int& vertexCnt);

	bool LoadNode(FbxNode* node);

	void Shotdown();

private:

	FbxManager* m_manager;
	// FbxImporter�� FbxSene�� Fbx���Ͽ� ä��ϴ�.
	FbxImporter* m_importer;
	FbxScene* m_scene;
	FbxMesh* m_mesh;
};
