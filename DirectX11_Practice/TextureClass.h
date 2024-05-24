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

    // Targa �̹��� ������ Dx �ؽ��ķ� ��ȯ�ϴ� ���
    bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*);
    void Shutdown();

    ID3D11ShaderResourceView* GetTexture();

    int GetWidth();
    int GetHeight();

private:

    // Targa �б� ���
    bool LoadTarga32Bit(char*);
    
private:

    // ���Ͽ��� ���� ���� ���� Targa������
    unsigned char* m_targaData;
    // DirectX�� �������� ����� ����ȭ�� �ؽ��� ������(������ GPU�� ����)
    ID3D11Texture2D* m_texture;
    // ���̴��� �׸��� �׸� �� �ؽ��� �����Ϳ� �׼����ϴ� ���ҽ� ���� ĭ
    ID3D11ShaderResourceView* m_textureView;
    // �ؽ����� ũ��
    int m_width, m_height;
};
