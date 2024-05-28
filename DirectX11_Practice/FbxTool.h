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


	bool Load(const char* fileName, VertexType** vertices, unsigned int** indices);
	
	// 재귀로 메시 노드를 찾아 m_mesh에 저장합니다.
	bool FindMesh(FbxNode* node);

	bool SaveVertexData(VertexType** vertices, unsigned int** indices);

	void Shotdown();

private:

	FbxManager* m_manager;
	// FbxImporter는 FbxSene을 Fbx파일에 채웁니다.
	FbxImporter* m_importer;
	FbxScene* m_scene;
	FbxMesh* m_mesh;
};
