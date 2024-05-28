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

	// 1. ������ �ʱ�ȭ
	ret = m_importer->Initialize(fileName, -1, m_manager->GetIOSettings());
	if (ret == false)
	{
		std::cout << m_importer->GetStatus().GetErrorString();
		return false;
	}

	m_importer->Import(m_scene);
	// ����Ʈ �� �����ʹ� �����Ͽ� �޷θ� ��뷮�� ���Դϴ�.
	m_importer->Destroy();

	// 2. �޽� ����
	ret = FindMesh(m_scene->GetRootNode());
	if (ret == false) return false;

	// 3. �޽��� ���� ������(��ġ, �ε���) ����
	SaveVertexData();

	return false;
}


bool FbxTool::FindMesh(FbxNode* node)
{
	// ã�� ��尡 �޽�Ÿ���̸� �޽ÿ� ���� �� ��ȯ�մϴ�.
	cout << node->GetTypeName() << endl;
	FbxNodeAttribute* attribute = node->GetNodeAttribute();
	if (node->GetNodeAttribute() != nullptr)
	{
		if (node->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			m_mesh = node->GetMesh();
			return true;
		}
	}
	
	// ��忡 �ڽĸ�ŭ for�� ����
	int childCnt = node->GetChildCount();
	for (int i = 0; i < childCnt; ++i)
	{
		if (FindMesh(node->GetChild(i)) == true)
		{
			return true;
		}
	}
	
	// �� ��ȸ�ص� �޽ð� ������ false��ȯ�մϴ�.
	return false;
}

bool FbxTool::SaveVertexData()
{
	// ������ ����
	int vertexCnt = m_mesh->GetControlPointsCount();
	// ���� ������ ����
	m_pos = new XMFLOAT3[vertexCnt];
	// ������ �迭 ������ ���
	FbxVector4* position = m_mesh->GetControlPoints();

	// ���� ��ġ ���Ͽ� ����
	for (int i = 0; i < vertexCnt; ++i)
	{
		float x = static_cast<float>(position[i][0]);
		float y = static_cast<float>(position[i][1]);
		float z = static_cast<float>(position[i][2]);
		m_pos[i] = XMFLOAT3(x, y, z);
	}

	// �ε��� ����
	int indexCnt = m_mesh->GetPolygonVertexCount() * 3;
	m_idx = new unsigned int[indexCnt];

	// ���� �ε��� ���ϱ� , // �ﰢ�� ���� ��ŭ �ݺ�
	int polygonCnt = m_mesh->GetPolygonCount();
	for (int i = 0; i < polygonCnt; ++i)
	{ 
		// �ﰢ���� �� �� 
		for (int j = 0; j < 3; ++j)
		{
			m_idx[(i*3) + j] = m_mesh->GetPolygonVertex(i, j);
		}
	}

	return true;
}


void FbxTool::Shotdown()
{
	m_scene->Destroy();
	m_importer->Destroy();
	m_manager->Destroy();
}
