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
	// 관리자 객체 생성
	m_manager = FbxManager::Create();

	// 가져올 씬의 요소(camera, light, mesh, texture, ...) 세팅
	FbxIOSettings* ios = FbxIOSettings::Create(m_manager, IOSROOT);
	m_manager->SetIOSettings(ios);
	
	// FbxImporter 개체 생성
	// FbxImporter는 FbxSene을 Fbx파일에 채웁니다.
	FbxImporter* importer = FbxImporter::Create(m_manager, "");

	// FbxImporter 초기화 
	if (importer->Initialize("fff.fbx", -1, m_manager->GetIOSettings()) == false)
	{
		// importer->GetStatus().GetErrorString()
		return;
	}

	// 씬 생성
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
