#include "TextureClass.h"

///////////////////////////////////////////////////////////////////////////////
// Filename: textureclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "textureclass.h"

TextureClass::TextureClass()
{
	m_targaData = 0;
	m_texture = 0;
	m_textureView = 0;
}

bool TextureClass::Initialize(ID3D11Device * device, ID3D11DeviceContext * deviceContext, char* filename)
{
	bool result;
	//int height, width;
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT hResult;
	unsigned int rowPitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	// targa image data�� �޸𸮿� �����մϴ�.
	result = LoadTarga32Bit(filename);
	if (!result)
	{
		return false;
	}


	// Targa �̹��� �������� ���̿� �ʺ� ���
	textureDesc.Height = m_height;
	textureDesc.Width = m_width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	// �ȼ� ������ 32��Ʈ RGBA �ؽ�ó�� ����
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// SampleDesc�� �⺻������ �����մϴ�
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// �� �ؽ��ĸ� ����
	hResult = device->CreateTexture2D(&textureDesc, NULL, &m_texture);
	if (FAILED(hResult))
	{
		return false;
	}

	// ���� ũ���� �޸�
	rowPitch = (m_width * 4) * sizeof(unsigned char);

	// �� �ؽ��Ŀ� Targa�̹����� �ֽ��ϴ�.
	deviceContext->UpdateSubresource(m_texture, 0, NULL, m_targaData, rowPitch, 0);

	// shader resource view description�� �����մϴ�.
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// �ؽ��ĸ� ���� shader resource view�� �����մϴ�.
	hResult = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
	if (FAILED(hResult))
	{
		return false;
	}

	// �� �ؽ����� mipmaps�� ����ϴ�.
	deviceContext->GenerateMips(m_textureView);

	// Release the targa image data now that the image data has been loaded into the texture.
	delete[] m_targaData;
	m_targaData = 0;

	return true;
}

void TextureClass::Shutdown()
{
	// Release the texture view resource.
	if (m_textureView)
	{
		m_textureView->Release();
		m_textureView = 0;
	}

	// Release the texture.
	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	// Release the targa data.
	if (m_targaData)
	{
		delete[] m_targaData;
		m_targaData = 0;
	}

	return;
}

ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return m_textureView;
}


bool TextureClass::LoadTarga32Bit(char* filename)
{
	int error, bpp, imageSize, index, i, j, k;
	FILE* filePtr;
	unsigned int count;
	TargaHeader targaFileHeader;
	unsigned char* targaImage;


	// ���̳ʸ� ���� targa ������ ���ϴ�.
	error = fopen_s(&filePtr, filename, "rb");
	if (error != 0)
	{
		return false;
	}

	// targaFileHeader ������ ������ �����մϴ�.
	count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// ������� �߿��� ������ �����ɴϴ�.
	m_height = (int)targaFileHeader.height;
	m_width = (int)targaFileHeader.width;
	bpp = (int)targaFileHeader.bpp;

	// 32bit Targa �̹������� Ȯ���մϴ�.
	if (bpp != 32)
	{
		return false;
	}

	// 32bit �̹����� ������ ũ�⸦ ����մϴ�.
	imageSize = m_width * m_height * 4; // �� �ȼ��� 4����Ʈ�̱� ������ 4�� �����ݴϴ�.

	// Targa �̹��� �����͸� ������ �޸𸮸� �Ҵ��մϴ�.
	targaImage = new unsigned char[imageSize];

	// Targa �̹��� �����͸� �о�ɴϴ�.
	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	// ������ �ݽ��ϴ�.
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	// targa destination data�� ���� �޸� �Ҵ��� �մϴ�.
	m_targaData = new unsigned char[imageSize];

	// �츮�� �� �ؽ��İ� ä������ ������ �ε��� �Դϴ�.
	index = 0;

	// Targa �ؽ��ĸ� Ž���ϱ� ������ �ε��� �Դϴ�.
	k = (m_width * m_height * 4) - (m_width * 4);

	// targa �̹��� �����͸� �ùٸ� ������ targa ��� �迭�� �����մϴ�.
	for (j = 0; j < m_height; j++)
	{
		for (i = 0; i < m_width; i++)
		{
			m_targaData[index + 0] = targaImage[k + 2];  // Red.
			m_targaData[index + 1] = targaImage[k + 1];  // Green.
			m_targaData[index + 2] = targaImage[k + 0];  // Blue
			m_targaData[index + 3] = targaImage[k + 3];  // Alpha

			// targa �����Ϳ� ���� �ε����� ������ŵ�ϴ�.
			k += 4;
			index += 4;
		}

		// targa �̹��� ������ �ε����� �Ųٷ� �о �� ���� �κ��� ���� ������ �ٽ� �����մϴ�.
		k -= (m_width * 8);
	}

	// ��� �迭�� ����Ǿ����Ƿ� ���� targa �̹��� �����͸� �����մϴ�.
	delete[] targaImage;
	targaImage = 0;

	return true;
}


int TextureClass::GetWidth()
{
	return m_width;
}


int TextureClass::GetHeight()
{
	return m_height;
}
