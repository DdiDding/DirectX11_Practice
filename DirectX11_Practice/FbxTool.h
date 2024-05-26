#pragma once
#include <fbxsdk.h>

class FbxTool
{
public:

	FbxTool();
	~FbxTool() = default;

	bool Initialize();

	bool Load(const char* fileName);
	
	bool LoadNode(FbxNode* node);

	void Shotdown();

private:

	FbxManager* m_manager;
	// FbxImporter�� FbxSene�� Fbx���Ͽ� ä��ϴ�.
	FbxImporter* m_importer;
	FbxScene* m_scene;
};
