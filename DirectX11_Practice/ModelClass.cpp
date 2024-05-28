#include "ModelClass.h"
#include <iostream>
#include "FbxTool.h"

ModelClass::ModelClass()
{
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
	m_Texture = nullptr;
	m_model = nullptr;
	m_fbx = nullptr;
}

bool ModelClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* modelFilename, char* textureFilename)
{
	bool result;

	m_fbx = new FbxTool();
	m_fbx->Initialize();

	LoadFBX(modelFilename);

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	// Load the texture for this model.
	/*result = LoadTexture(device, deviceContext, textureFilename);
	if (!result)
	{
		return false;
	}*/

	return true;
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// vertex and index buffers�� ������ ������ ���ο� ��ġ�մϴ�.
	RenderBuffers(deviceContext);

	return;
}

int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

// ���� �������Ϸ��� �ؽ��� ���̴��� �� �ؽ��Ŀ� �׼��� �ؾ��մϴ�.
ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	///////////////////////////////////////////////////////////////////////////////
	// Vertex Buffer ����
	{
		if (m_fbx->GetVertexPos() == nullptr) return false;
		m_vertexCount = _msize(m_fbx->GetVertexPos()) / sizeof(XMFLOAT3);
		m_vertices = new VertexColor[m_vertexCount];

		// Load the vertex array and index array with data.
 		for (int i = 0; i < m_vertexCount; i++)
		{
			m_vertices[i].position = m_fbx->GetVertexPos()[i];
			m_vertices[i].color = XMFLOAT4(i * 0.001, 0, 1, 1);
		}


		// Test vertex
		/*
		m_vertexCount = 3;
		m_vertices = new VertexColor[3];

		m_vertices[0].position = XMFLOAT3(-1, 0, 0);
		m_vertices[0].color = XMFLOAT4(1, 0, 0, 1);

		m_vertices[1].position = XMFLOAT3(0, 1, 0);
		m_vertices[1].color = XMFLOAT4(1, 0, 0, 1);

		m_vertices[2].position = XMFLOAT3(1, 0, 0);
		m_vertices[2].color = XMFLOAT4(1, 0, 0, 1);*/


		// Set up the description of the static vertex buffer.
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(VertexColor) * m_vertexCount;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		// Give the subresource structure a pointer to the vertex data.
		vertexData.pSysMem = m_vertices;
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		// Now create the vertex buffer.
		result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
		if (FAILED(result))
		{
			return false;
		}

		// Release the arrays now that the vertex and index buffers have been created and loaded.
		delete[] m_vertices;
		m_vertices = 0;
	}
	
	///////////////////////////////////////////////////////////////////////////////
	// Index Buffer ����
	{
		m_indices = m_fbx->GetVertexIdx();
		m_indexCount = _msize(m_indices) / sizeof(unsigned int);


		/*m_indexCount = 3;
		m_indices = new unsigned int[6];

		m_indices[0] = 0;
		m_indices[1] = 1;
		m_indices[2] = 2;*/


		// Set up the description of the static index buffer.
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(unsigned int) * m_indexCount;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;

		// Give the subresource structure a pointer to the index data.
		indexData.pSysMem = m_indices;
		indexData.SysMemPitch = 0;
		indexData.SysMemSlicePitch = 0;

		// Create the index buffer.
		result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
		if (FAILED(result))
		{
			return false;
		}

		delete[] m_indices;
		m_indices = 0;
	}

	return true;
}

void ModelClass::ShutdownBuffers()
{
	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}

	return;
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexColor);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool ModelClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;

	// TextureClass��ü�� �����ϰ� �ʱ�ȭ �մϴ�.
	m_Texture = new TextureClass;

	result = m_Texture->Initialize(device, deviceContext, filename);
	if (!result)
	{
		return false;
	}

	return true;
}



bool ModelClass::LoadModel(char* filename)
{
	ifstream fin;
	char input;
	int i;

	// ���� ����.
	fin.open(filename);

	// ������ ���� ������ ����.
	if (fin.fail())
	{
		return false;
	}

	// ���� ������ ������ ������ �о���ϴ�.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// ������ ���� �о� ���� ������ m_vertexCount�� �����ɴϴ�.
	fin >> m_vertexCount;

	// �ε����� ������ ���� ������ �����ϴ�.
	m_indexCount = m_vertexCount;

	// �о�� ���� ������ ����Ͽ� �迭�� ����� �����Ϳ� �ֽ��ϴ�.
	m_model = new ModelType[m_vertexCount];

	// ���� �����Ͱ� ������ ������ �о���ϴ�.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	// ���鹮�� �о��
	fin.get(input);
	fin.get(input);

	// ���� �����͸� �н��ϴ� 
	for (i = 0; i < m_vertexCount; i++)
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	// ���� �ݱ�
	fin.close();

	return true;
}

bool ModelClass::LoadFBX(char* fbxFileName)
{
	assert(m_fbx);

	XMFLOAT3** vertexPos = nullptr;
	int** vertexIdx = nullptr;
	m_vertexCount = 0;

	if (m_fbx->Load(fbxFileName) == false) return false;

	return false;
}


void ModelClass::Shutdown()
{
	ReleaseModel();

	ReleaseTexture();

	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	return;
}


void ModelClass::ReleaseModel()
{
	if (m_model)
	{
		delete[] m_model;
		m_model = 0;
	}

	return;
}

void ModelClass::ReleaseTexture()
{
	// Release the texture object.
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = nullptr;
	}

	return;
}