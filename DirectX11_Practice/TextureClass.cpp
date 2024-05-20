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

	// targa image data를 메모리에 저장합니다.
	result = LoadTarga32Bit(filename);
	if (!result)
	{
		return false;
	}


	// Targa 이미지 데이터의 높이와 너비를 사용
	textureDesc.Height = m_height;
	textureDesc.Width = m_width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	// 픽셀 형식을 32비트 RGBA 텍스처로 설정
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// SampleDesc를 기본값으로 설정합니다
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// 빈 텍스쳐를 생성
	hResult = device->CreateTexture2D(&textureDesc, NULL, &m_texture);
	if (FAILED(hResult))
	{
		return false;
	}

	// 가로 크기의 메모리
	rowPitch = (m_width * 4) * sizeof(unsigned char);

	// 빈 텍스쳐에 Targa이미지를 넣습니다.
	deviceContext->UpdateSubresource(m_texture, 0, NULL, m_targaData, rowPitch, 0);

	// shader resource view description를 설정합니다.
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// 텍스쳐를 위한 shader resource view를 생성합니다.
	hResult = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
	if (FAILED(hResult))
	{
		return false;
	}

	// 이 텍스쳐의 mipmaps을 만듭니다.
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


	// 바이너리 모드로 targa 파일을 엽니다.
	error = fopen_s(&filePtr, filename, "rb");
	if (error != 0)
	{
		return false;
	}

	// targaFileHeader 형식의 내용을 저장합니다.
	count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// 헤더에서 중요한 정보를 가져옵니다.
	m_height = (int)targaFileHeader.height;
	m_width = (int)targaFileHeader.width;
	bpp = (int)targaFileHeader.bpp;

	// 32bit Targa 이미지인지 확인합니다.
	if (bpp != 32)
	{
		return false;
	}

	// 32bit 이미지의 데이터 크기를 계산합니다.
	imageSize = m_width * m_height * 4; // 각 픽셀이 4바이트이기 때문에 4를 곱해줍니다.

	// Targa 이미지 데이터를 저장할 메모리를 할당합니다.
	targaImage = new unsigned char[imageSize];

	// Targa 이미지 데이터를 읽어옵니다.
	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	// 파일을 닫습니다.
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	// targa destination data를 위한 메모리 할당을 합니다.
	m_targaData = new unsigned char[imageSize];

	// 우리의 빈 텍스쳐가 채워지기 시작할 인덱스 입니다.
	index = 0;

	// Targa 텍스쳐를 탐색하기 시작할 인덱스 입니다.
	k = (m_width * m_height * 4) - (m_width * 4);

	// targa 이미지 데이터를 올바른 순서로 targa 대상 배열에 복사합니다.
	for (j = 0; j < m_height; j++)
	{
		for (i = 0; i < m_width; i++)
		{
			m_targaData[index + 0] = targaImage[k + 2];  // Red.
			m_targaData[index + 1] = targaImage[k + 1];  // Green.
			m_targaData[index + 2] = targaImage[k + 0];  // Blue
			m_targaData[index + 3] = targaImage[k + 3];  // Alpha

			// targa 데이터에 대한 인덱스를 증가시킵니다.
			k += 4;
			index += 4;
		}

		// targa 이미지 데이터 인덱스를 거꾸로 읽어서 열 시작 부분의 이전 행으로 다시 설정합니다.
		k -= (m_width * 8);
	}

	// 대상 배열에 복사되었으므로 이제 targa 이미지 데이터를 해제합니다.
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
