#include "FbxTool.h"

#if _DEBUG
#pragma comment (lib, "C:\\Program Files\\Autodesk\\FBX\\FBX SDK\\2020.3.7\\lib\\x64\\debug\\libfbxsdk-md.lib")
#pragma comment (lib, "C:\\Program Files\\Autodesk\\FBX\\FBX SDK\\2020.3.7\\lib\\x64\\debug\\libxml2-md.lib")
#pragma comment (lib, "C:\\Program Files\\Autodesk\\FBX\\FBX SDK\\2020.3.7\\lib\\x64\\debug\\zlib-md.lib")
#else
#pragma comment (lib, "C:\\Program Files\\Autodesk\\FBX\\FBX SDK\\2020.3.7\\lib\\x64\\release\\libfbxsdk-md.lib")
#pragma comment (lib, "C:\\Program Files\\Autodesk\\FBX\\FBX SDK\\2020.3.7\\lib\\x64\\release\\libxml2-md.lib")
#pragma comment (lib, "C:\\Program Files\\Autodesk\\FBX\\FBX SDK\\2020.3.7\\lib\\x64\\release\\zlib-md.lib")
#endif

FbxTool::FbxTool()
{
	m_manager = nullptr;
}

void FbxTool::Initialize()
{
	// ������ ��ü ����
	m_manager = FbxManager::Create();

	// ������ ���� ���(camera, light, mesh, texture, ...) ����
	FbxIOSettings* ios = FbxIOSettings::Create(m_manager, IOSROOT);
	m_manager->SetIOSettings(ios);
	
	// FbxImporter ��ü ����
	// FbxImporter�� FbxSene�� Fbx���Ͽ� ä��ϴ�.
	FbxImporter* importer = FbxImporter::Create(m_manager, "");

	// FbxImporter �ʱ�ȭ 
	if (importer->Initialize("fff.fbx", -1, m_manager->GetIOSettings()) == false)
	{
		// importer->GetStatus().GetErrorString()
		return;
	}

	// �� ����
	FbxScene* scene = FbxScene::Create(m_manager, "myScene");

	importer->Import(scene);

	importer->Destroy();
}

bool FbxTool::Load(const char* fileName)
{
	return false;
}

void FbxTool::Shotdown()
{

}
