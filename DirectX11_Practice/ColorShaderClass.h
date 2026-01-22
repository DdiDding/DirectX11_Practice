#pragma once
/********************************************************/
/* includes */
/********************************************************/
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
using namespace DirectX;
using namespace std;


/********************************************************/
/* Class */
/********************************************************/
// ¼ÎÀÌ´õ¸¦ ºÒ·¯¿Í È£ÃâÇÏ´Âµ¥ »ç¿ëµÉ Å¬·¡½º
class ColorShaderClass
{
private:
	//VS¿¡ »ç¿ëµÉ cBufferÀÇ Á¤ÀÇ, ±×·¸±â ¶§¹®¿¡ ¼ÎÀÌ´õÀÇ ±¸Á¶¿Í °°¾Æ¾ßÇÔ
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

public:
	ColorShaderClass();
	ColorShaderClass(const ColorShaderClass&) = default;
	~ColorShaderClass() = default;
	// ¼ÎÀÌ´õÀÇ ÃÊ±âÈ­ ÇÔ¼ö¸¦ È£­ŒÇÑ´Ù.
	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	// ¼ÎÀÌ´õ ³»ºÎÀÇ ¸Å°³º¯¼ö¸¦ ¼³Á¤ÇÑ´Ù?
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
};

