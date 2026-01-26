#include "ColorShaderClass.h"

ColorShaderClass::ColorShaderClass()
{
    m_vertexShader = nullptr;
    m_pixelShader = nullptr;
    m_layout = nullptr;
    m_matrixBuffer = nullptr;
}

bool ColorShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;
	wchar_t vsFilename[128];
	wchar_t psFilename[128];
	int error;

	// 각 셰이더의 경로를 설정한다.
	error = wcscpy_s(vsFilename, 128, L"../Shaders/ColorVS.hlsl");
	if (error != 0) return false;

	error = wcscpy_s(psFilename, 128, L"../Shaders/ColorPS.hlsl");
	if (error != 0) return false;

	// 각 셰이더 초기화
	result = InitializeShader(device, hwnd, vsFilename, psFilename);
	if (!result) return false;

	return true;
}

void ColorShaderClass::Shutdown()
{
	ShutdownShader();
	return;
}

bool ColorShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix)
{
	bool result;

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if (!result) return false;

	// 준비된 버퍼를 셰이더와 함께 랜더링한다.
	// 
	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}

bool ColorShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage = nullptr;
	
	// VS 셰이더 컴파일
	ID3D10Blob* vertexShaderBuffer = nullptr;
	{
		// 정점 셰이더 코드를 컴파일 합니다.
		result = D3DCompileFromFile(vsFilename, NULL, NULL, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
			&vertexShaderBuffer, &errorMessage);

		if (FAILED(result))
		{
			// If the shader failed to compile it should have writen something to the error message.
			if (errorMessage)
			{
				OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
			}
			// If there was  nothing in the error message then it simply could not find the shader file itself.
			else
			{
				MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
			}

			return false;
		}
	}
	
	// 픽셀 셰이더 코드를 컴파일 합니다.
	ID3D10Blob* pixelShaderBuffer = nullptr;
	{
		result = D3DCompileFromFile(psFilename, NULL, NULL, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
			&pixelShaderBuffer, &errorMessage);
		if (FAILED(result))
		{
			// If the shader failed to compile it should have writen something to the error message.
			if (errorMessage)
			{
				OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
			}
			// If there was nothing in the error message then it simply could not find the file itself.
			else
			{
				MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
			}

			return false;
		}
	}
	

	// 각 셰이더가 버퍼에 성공적으로 컴파일되면, 버퍼를 사용해 셰이더 객체를 직접 생성한다.
	//포인터를 사용해 셰이더 객체에 접근한다.

	// buffer를 이용해 VS 객체를 만든다.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result)) return false;

	// buffer를 이용해 PS 객체를 만든다.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result)) return false;

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	// 셰이더에 정점을 넘겨줄텐데, 각 정점이 어떻게 해석이 되는지 작성
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	{
		polygonLayout[0].SemanticName = "POSITION";
		polygonLayout[0].SemanticIndex = 0;
		polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		polygonLayout[0].InputSlot = 0;
		polygonLayout[0].AlignedByteOffset = 0; // 버퍼에서의 데이터 간격
		polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[0].InstanceDataStepRate = 0;

		polygonLayout[1].SemanticName = "COLOR";
		polygonLayout[1].SemanticIndex = 0;
		polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		polygonLayout[1].InputSlot = 0;
		polygonLayout[1].AlignedByteOffset =	D3D11_APPEND_ALIGNED_ELEMENT;
		polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[1].InstanceDataStepRate = 0;
	}

	//device를 이용하여 Inputlayout생성
	// Layout 개수를 센다.
	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result)) return false;

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	// 상수버퍼 설정
	D3D11_BUFFER_DESC matrixBufferDesc;
	{
		// 정점 셰이더에 있는 상수 버퍼(cbuffer)의 설명을 설정합니다.
		//상수 버퍼의 사용 방식을 지정합니다. 여기서는 매 프레임마다 업데이트되므로 D3D11_USAGE_DYNAMIC을 사용합니다.
		matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		// ByteWidth는 상수 버퍼의 크기를 바이트 단위로 지정합니다. 이 경우에는 MatrixBufferType 구조체의 크기를 사용합니다.
		matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
		// BindFlags는 버퍼가 어떤 유형의 버퍼가 될지 지정합니다. 여기서는 상수 버퍼로 사용하므로 D3D11_BIND_CONSTANT_BUFFER를 사용합니다.
		matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		// CPUAccessFlags는 Usage와 맟춰 사용해야하기에 여기서 D3D11_CPU_ACCESS_WRITE를 사용합니다.
		matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		matrixBufferDesc.MiscFlags = 0;
		matrixBufferDesc.StructureByteStride = 0;

		// Create the constant buffer pointer so we can access the vertex	shader constant buffer from within this class.
		result = device->CreateBuffer(&matrixBufferDesc, NULL,	&m_matrixBuffer);
		if (FAILED(result)) return false;
	}

	return true;
}

void ColorShaderClass::ShutdownShader()
{
	// Release the matrix constant buffer.
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// Release the layout.
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// Release the pixel shader.
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// Release the vertex shader.
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}

void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize, i;
	ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}

bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix)
{
	HRESULT result;
	
	// Transpose the matrices to prepare them for the shader.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// Lock the constant buffer so it can be written to.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) return false;

	// Get a pointer to the data in the constant buffer.
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// Unlock the constant buffer.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	unsigned int bufferNumber = 0;

	// Finanly set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}

void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}
