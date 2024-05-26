#include "FbxTool.h"
#include <iostream>

using namespace std;

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
	m_scene = nullptr;

	
}

bool FbxTool::Initialize()
{
	// ������ ��ü ����
	m_manager = FbxManager::Create();
	if (!m_manager)
	{
		std::cout << "Error: Unable to create FBX Manager!\n";
		return false;
	}

	// ������ ���� ���(camera, light, mesh, texture, ...) ����
	FbxIOSettings* ios = FbxIOSettings::Create(m_manager, IOSROOT);
	m_manager->SetIOSettings(ios);

	// ������ �������� ���� ����
	(*(m_manager->GetIOSettings())).SetBoolProp(IMP_FBX_MATERIAL, false);
	(*(m_manager->GetIOSettings())).SetBoolProp(IMP_FBX_TEXTURE, false);
	(*(m_manager->GetIOSettings())).SetBoolProp(IMP_FBX_LINK, false);
	(*(m_manager->GetIOSettings())).SetBoolProp(IMP_FBX_SHAPE, true);
	(*(m_manager->GetIOSettings())).SetBoolProp(IMP_FBX_GOBO, false);
	(*(m_manager->GetIOSettings())).SetBoolProp(IMP_FBX_ANIMATION, false);
	(*(m_manager->GetIOSettings())).SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, false);

	
	

	m_importer = FbxImporter::Create(m_manager, "");

	// �� ����
	m_scene = FbxScene::Create(m_manager, "My Scene");
	if (!m_scene)
	{
		std::cout << "Error : Unable to create FBX scene\n";
		return false;
	}

	return true;
}

bool FbxTool::Load(const char* fileName)
{
	bool ret;

	// ������ �ʱ�ȭ
	ret = m_importer->Initialize(fileName, -1, m_manager->GetIOSettings());
	if (ret == false)
	{
		std::cout << m_importer->GetStatus().GetErrorString();
		return false;
	}

	m_importer->Import(m_scene);
	// ����Ʈ �� �����ʹ� �����Ͽ� �޷θ� ��뷮�� ���Դϴ�.
	m_importer->Destroy();

	FbxMesh* mesh;
	FbxNode* root = m_scene->GetRootNode();
	cout << root->GetName() << "//" << root->GetChildCount() << endl; //anim man
	mesh = root->GetMesh();

	FbxNode* child2_2 = root->GetChild(0)->GetChild(1);
	cout << child2_2->GetName() << "//" << child2_2->GetChildCount() << endl; //anim man -> mesh��!
	mesh = child2_2->GetMesh();

	cout << mesh->GetName()<<mesh->GetPolygonCount();


	return false;
}

bool FbxTool::LoadNode(FbxNode* node)
{
	FbxMesh* mesh = node->GetMesh();
	std::cout << mesh->GetName();

	return false;
}

void FbxTool::Shotdown()
{
	m_scene->Destroy();
	m_importer->Destroy();
	m_manager->Destroy();
}
