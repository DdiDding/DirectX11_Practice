#include "D3DClass.h"

D3DClass::D3DClass()
{
	m_swapChain = nullptr;
	m_device = nullptr;
	m_deviceContext = nullptr;
	m_renderTargetView = nullptr;
	m_depthStencilBuffer = nullptr;
	m_depthStencilState = nullptr;
	m_depthStencilView = nullptr;
	m_rasterState = nullptr;
}
	

bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{
	HRESULT result;
	// DXGI ��ü�� �����ϰ� �����ϴ� ����� �Ѵ�.
	IDXGIFactory* factory;
	// IDXGIAdapter : �׷��� ī���� Ư�� ��ɰ� ������ �����ϴ� �������̽�
	// �׷��� ī���� ������ ���� �ǰ� �ش� �׷��� ī��� ���õ� �۾��� ���� ����
	IDXGIAdapter* adapter;
	// IDXGIOutput : �׷��� ī���� �����ġ(�����, ...)�� ������ �����ϴ� �������̽�
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator;
	unsigned long long stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	float fieldOfView, screenAspect;


	// ���� ����ȭ ����
	m_vsync_enabled = vsync;

	//////////////////////////////////////////////////////////////////////////////////////////////
	// �ֻ��� ���� �� �˾Ƴ���
	//////////////////////////////////////////////////////////////////////////////////////////////

	// DirectX graphics Infrastructure(DXGI) factory�� ����
	// DXGI�� �ϴ� ���α׷��� �׷��� ��� ��ġ(�����, �׷���ī�� ..) ���̿��� �����ִ� ����̶�� ��������.
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);

	if (FAILED(result))
	{
		return false;
	}

	// ��� ���� DXGI factory�� ����� 0��° ����ϴ� �׷���ī���� IDXGIAdapter�� �����Ѵ�.
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	// IDXGIAdapter�� �̿��� �׷��� ī���� ��� ������ 0��° �����ġ(�����)�� ������ �����Ѵ�.
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	// ���� ������ DXGI_FORMAT_R8G8B8A8_UNORM�� ���÷��� ����� ���� ������ numModes�� �����Ѵ�.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// ��� ������ ���÷��� ����� ���� ���� ��ŭ �迭�� �����Ѵ�.
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	// numModes�� �� ��ŭ ���÷��� ��� ������ displayModeList���� �����´�.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
	{
		return false;
	}

	// ���÷��� ��� ��Ͽ��� ������� ũ�⿡ �´� ���÷��� ��带 ã�´�.
	// ��ġ�ϴ� ���÷��� ��带 ã���� �ֻ����� ���� �и� �����Ѵ�.
	for (i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	// �׷��� ī�� �̸��� vram �뷮 �˾Ƴ���
	//////////////////////////////////////////////////////////////////////////////////////////////
	
	// DXGI_ADAPTER_DESC adapterDesc;
	// �׷��� ī���� ������ �����´�.
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}

	// dedicated video card memory(VRAM)�� �ް�����Ʈ ������ �����մϴ�.
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// �׷��� ī���� �̸��� Char�� �迭�� ��ȯ�Ͽ� �����մϴ�.
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	// Release the display mode list.
	delete[] displayModeList;
	displayModeList = 0;

	// Release the adapter output.
	adapterOutput->Release();
	adapterOutput = 0;

	// Release the adapter.
	adapter->Release();
	adapter = 0;

	// Release the factory.
	factory->Release();
	factory = 0;

	//////////////////////////////////////////////////////////////////////////////////////////////
	// ���� ü��
	//////////////////////////////////////////////////////////////////////////////////////////////

	// ����ü���� ���ð�(DXGI_SWAP_CHAIN_DESC)�� �����ϱ��� 0���� �ʱ�ȭ �Ѵ�.
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// ���۸� ���� 2��, Ȥ�� 3���� ���������� �ϴ� Ʃ�丮�󿡼��� ���� ������ 1��, �̱� ���۸����� ���۽�Ų��.
	swapChainDesc.BufferCount = 1;

	// ����(��ȭ��)�� ũ�� ����.
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// ����(��ȭ��)�� �ȼ� ������ ����.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// ������ �ֻ����� ����, ���� ����ȭ ���ο� ���� �бⰡ ������.
	if (m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	// ���� ����ȭ�� �ƴ϶�� �ֻ����� ������ ��������.
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// ������ �뵵�� �����ϴ°ǵ� �ϴ� Ʃ�丮�󿡼��� �������� �������� �����ߴ�.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// ������ �ڵ��� ����.
	swapChainDesc.OutputWindow = hwnd;

	// ��Ƽ ���ø�(��Ƽ ���ϸ����) �� ����.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// ��ü ȭ���� ���� �б�.
	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// ���۸� ���������� ���ʴ�� �׸�����, ¦�� �ٺ��� �׸�����, ..� ���� ����, Ʃ�丮�󿡼��� �⺻�� ���.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	// �����ϸ��� ���� ����, Ʃ�丮�󿡼��� �⺻�� ���.
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// ������ ��ü ����� ���� ���ε� ������ ���� ���۶� �ǹ� ����.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// �÷��׸� �������� �ʴ´� = �ϴ� �߿����� �ʴ�.
	swapChainDesc.Flags = 0;

	// ����� DirectX�� ������ ����
	featureLevel = D3D_FEATURE_LEVEL_11_0;
	
	// ���� ������ DXGI_SWAP_CHAIN_DESC�� �̿��� ����ü��, Direct3D device, Direct3D device context�� �����Ѵ�.
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
			D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	if (FAILED(result))
	{
		return false;
	}

	// ����ü���� ���� �����͸� �����´�.
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
	{
		return false;
	}

	// ���� Ÿ�� ��(���� ����)�� ���۸� �����ϸ鼭 �����Ѵ�.
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	// ���� �̿밡ġ�� ����� ������ �����͸� �޸� �����Ѵ�.
	backBufferPtr->Release();
	backBufferPtr = nullptr;
	
	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// ���۴� �ᱹ 2D �ؽ����� ���̱⿡ ���۷� ����� 2D �ؽ��ĺ��� ������̴�.
	// Set up the description of the depth buffer.
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	// 32bit�� 24bit�� Depth buffer��, 8bit�� Stencil buffer�� ����Ѵٴ� ���̴�.
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;
	
	// �ϳ��� 2D Texture�� ����, ���ٽ� ���۷� ����Ѵ�.
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Initialize the description of the stencil state.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// �ȼ��� ������ ���ϰ� �ִ� ��� ���ٽ� �۾�. 
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// �ȼ��� �ĸ��� ���ϰ� �ִ� ��� ���ٽ� �۾�. 
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	

	// depth stencil state ����
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result))
	{
		return false;
	}

	// depth stencil state ���
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	// depth stencil view �ʱ�ȭ
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// depth stencil view Desc ����
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// depth stencil view ����
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
	{
		return false;
	}

	// ���� Ÿ�� ��� ���� ���ٽ� ���۸� ��� ���� ���������ο� ���ε��մϴ�. 
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	//////////////////////////////////////////////////////////////////////////////////////////////
	// �����Ͷ�����
	//////////////////////////////////////////////////////////////////////////////////////////////
	
	// �������� ��� �׷������� �Ӽ��� Desc�� ä���ֱ�
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;



	// Desc�� �̿��� rasterizer state����
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(result))
	{
		return false;
	}

	// rasterizer state ������ ���� RS�ܰ迡 ����
	m_deviceContext->RSSetState(m_rasterState);

	// Viewport�� �ٸ� �׷��� ���ҽ��� �޸� Desc���� ���� �����Ѵ�.
	// ����Ʈ �Ӽ� ����
	m_viewport.Width = (float)screenWidth;
	m_viewport.Height = (float)screenHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;

	// viewport�� �����ϰ� ������ ���� RS�ܰ迡 ����.
	m_deviceContext->RSSetViewports(1, &m_viewport);

	
	// ���� ����� ���� ��ķ� �ʱ�ȭ �մϴ�. (���� ��ȯ�� ���)
	m_worldMatrix = XMMatrixIdentity();

	// 2D �������� ���� ���� ���� ����� �����Ѵ�.
	m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	// ���� ��� ����
	fieldOfView = 3.141592654f / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	// 3D�������� ���� ���� ��� ���� (���� ��ȯ�� ���)
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	return true;
}

void D3DClass::Shutdown()
{
	// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	if (m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = nullptr;
	}

	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = nullptr;
	}

	if (m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = nullptr;
	}

	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = nullptr;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = nullptr;
	}

	if (m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = nullptr;
	}

	if (m_device)
	{
		m_device->Release();
		m_device = nullptr;
	}

	if (m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = nullptr;
	}

	return;
}

void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];

	// Setup the color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the back buffer.
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// Clear the depth buffer.
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}


void D3DClass::EndScene()
{
	// Present the back buffer to the screen since rendering is complete.
	if (m_vsync_enabled)
	{
		// Lock to screen refresh rate.
		m_swapChain->Present(1, 0);
	}
	else
	{
		// Present as fast as possible.
		m_swapChain->Present(0, 0);
	}

	return;
}

ID3D11Device* D3DClass::GetDevice()
{
	return m_device;
}


ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return m_deviceContext;
}

void D3DClass::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}


void D3DClass::GetWorldMatrix(XMMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
	return;
}


void D3DClass::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
	return;
}

void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	return;
}

void D3DClass::SetBackBufferRenderTarget()
{
	// Bind the render target view and depth stencil buffer to the output render pipeline.
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	return;
}


void D3DClass::ResetViewport()
{
	// Set the viewport.
	m_deviceContext->RSSetViewports(1, &m_viewport);

	return;
}