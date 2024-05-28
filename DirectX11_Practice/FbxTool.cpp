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
	// 관리자 객체 생성
	m_manager = FbxManager::Create();
	if (!m_manager)
	{
		std::cout << "Error: Unable to create FBX Manager!\n";
		return false;
	}

	// 가져올 씬의 요소(camera, light, mesh, texture, ...) 세팅
	FbxIOSettings* ios = FbxIOSettings::Create(m_manager, IOSROOT);
	m_manager->SetIOSettings(ios);

	// 가져올 데이터의 종류 결정
	(*(m_manager->GetIOSettings())).SetBoolProp(IMP_FBX_MATERIAL, false);
	(*(m_manager->GetIOSettings())).SetBoolProp(IMP_FBX_TEXTURE, false);
	(*(m_manager->GetIOSettings())).SetBoolProp(IMP_FBX_LINK, false);
	(*(m_manager->GetIOSettings())).SetBoolProp(IMP_FBX_SHAPE, true);
	(*(m_manager->GetIOSettings())).SetBoolProp(IMP_FBX_GOBO, false);
	(*(m_manager->GetIOSettings())).SetBoolProp(IMP_FBX_ANIMATION, false);
	(*(m_manager->GetIOSettings())).SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, false);

	

	m_importer = FbxImporter::Create(m_manager, "");

	// 씬 생성
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

	// 1. 임포터 초기화
	ret = m_importer->Initialize(fileName, -1, m_manager->GetIOSettings());
	if (ret == false)
	{
		std::cout << m_importer->GetStatus().GetErrorString();
		return false;
	}

	m_importer->Import(m_scene);
	// 임포트 후 임포터는 해제하여 메로리 사용량을 줄입니다.
	m_importer->Destroy();

	// 2. 메시 저장
	ret = FindMesh(m_scene->GetRootNode());
	if (ret == false) return false;

	// 3. 메시의 정점 데이터(위치, 인덱스) 저장
	SaveVertexData();

	return false;
}


bool FbxTool::FindMesh(FbxNode* node)
{
	// 찾은 노드가 메시타입이면 메시에 저장 후 반환합니다.
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
	
	// 노드에 자식만큼 for문 진행
	int childCnt = node->GetChildCount();
	for (int i = 0; i < childCnt; ++i)
	{
		if (FindMesh(node->GetChild(i)) == true)
		{
			return true;
		}
	}
	
	// 다 순회해도 메시가 없으면 false반환합니다.
	return false;
}

bool FbxTool::SaveVertexData()
{
	// 정점의 개수
	int vertexCnt = m_mesh->GetControlPointsCount();
	// 정점 데이터 생성
	m_pos = new XMFLOAT3[vertexCnt];
	// 정점의 배열 포인터 얻기
	FbxVector4* position = m_mesh->GetControlPoints();

	// 정점 위치 구하여 저장
	for (int i = 0; i < vertexCnt; ++i)
	{
		float x = static_cast<float>(position[i][0]);
		float y = static_cast<float>(position[i][1]);
		float z = static_cast<float>(position[i][2]);
		m_pos[i] = XMFLOAT3(x, y, z);
	}

	// 인덱스 개수
	int indexCnt = m_mesh->GetPolygonVertexCount() * 3;
	m_idx = new unsigned int[indexCnt];

	// 정점 인덱스 구하기 , // 삼각형 개수 만큼 반복
	int polygonCnt = m_mesh->GetPolygonCount();
	for (int i = 0; i < polygonCnt; ++i)
	{ 
		// 삼각형의 세 점 
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
