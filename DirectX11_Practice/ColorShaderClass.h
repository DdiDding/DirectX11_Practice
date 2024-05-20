#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>

using namespace DirectX;
using namespace std;

class ColorShaderClass
{
private:

	// 이 구조체가 바로 정점 셰이더의 cbuffer 값을 수정할 데이터 유형
	// 당연히 cbuffer 형식과 정확히 동일해야함
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

	// 셰이더의 초기화 및 종료를 처리합니다.
	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	// 셰이더가 사용하는 매개변수를 설정하고 셰이더가 사용할 준비된 모델의 정점을 그립니다.
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX);

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