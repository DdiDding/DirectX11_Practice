#pragma once
#include <d3d11.h>
#include <stdio.h>

class TextureClass
{
private:

    struct TargaHeader
    {
        unsigned char data1[12];
        unsigned short width;
        unsigned short height;
        //
        unsigned char bpp;
        unsigned char data2;
    };

public:
    TextureClass();
    TextureClass(const TextureClass&) = default;
    ~TextureClass() = default;

    // Targa 이미지 파일을 Dx 텍스쳐로 변환하는 기능
    bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*);
    void Shutdown();

    ID3D11ShaderResourceView* GetTexture();

    int GetWidth();
    int GetHeight();

private:

    // Targa 읽기 기능
    bool LoadTarga32Bit(char*);
    
private:

    // 파일에서 직접 읽은 원시 Targa데이터
    unsigned char* m_targaData;
    // DirectX가 렌더링에 사용할 구조화된 텍스쳐 데이터(실제는 GPU에 있음)
    ID3D11Texture2D* m_texture;
    // 셰이더가 그림을 그릴 때 텍스쳐 데이터에 액세스하는 리소스 장착 칸
    ID3D11ShaderResourceView* m_textureView;
    // 텍스쳐의 크기
    int m_width, m_height;
};
