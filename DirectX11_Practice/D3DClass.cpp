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
	
	unsigned int i;
	int error;
	
	
	float fieldOfView, screenAspect;

	// vsync 설정을 저장합니다. 
	m_vsync_enabled = vsync;

	// 1. DirectX graphics interface factory 생성 & DXGI 인터페이스 생성
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	{
		// 1.1 팩토리 생성
		result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
		if (FAILED(result)) return false;

		// 1.2. 팩토리를 이용해 VGA 인터페이스 가져오기
		result = factory->EnumAdapters(0, &adapter);
		if (FAILED(result)) return false;

		// 1.3. IDXGIAdapter 이용해 IDXGIOutput 가져오기
		// Enumerate the primary adapter output (monitor).
		result = adapter->EnumOutputs(0, &adapterOutput);
		if (FAILED(result)) return false;
	}


	// 2. 주사율 가져오기
	UINT numModes = 0;
	DXGI_MODE_DESC* displayModeList;
	unsigned int numerator, denominator;
	{
		// 2.1. 디스플레이 모드 개수 가져오기
		result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
		if (FAILED(result)) return false;

		// 2.2. 가져온 개수만큼 디스플레이 모드 정보 저장할 배열 생성
		// Create a list to hold all the possible display modes for this monitor/video card combination.
		displayModeList = new DXGI_MODE_DESC[numModes];
		if (FAILED(result)) return false;

		// 2.3. 배열에 정보 삽입
		result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
		if (FAILED(result)) return false;

		// 2.4 제공 받은 정보 이용하여 모니터에 맞는 주사율 분수 형태로 저장
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
	}

	
	// 3. VGA의 이름과 vram의 용량을 저장한다.
	// VGA의 정보 구조체
	DXGI_ADAPTER_DESC adapterDesc;
	unsigned long long stringLength;
	{
		// 3.1. VGA의 정보를 가져오기
		result = adapter->GetDesc(&adapterDesc);
		if (FAILED(result)) return false;

		// 3.2. VRAM을 MB단위로 저장한다.
		m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

		// 3.3. VGA 이름을 문자열형태로 char array에 저장한다.
		error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
		if (FAILED(result)) return false;
	}


	// 4.DXGI에서 가져올 정보를 모두 가져왔으니 가져온 자원을 해제한다.
	{
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
	}
	

	// 5. 스왑체인 생성
	// swapChainDesc : 스왑체인의 설정값을 포함하는 구조체, 사용하기 전에 0으로 초기화 한다.
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	{
		// 5.1 스왑체인의 설정 값 정의

		// 튜토리얼에선 우선 버퍼 개수를 1개인, 싱글 버퍼링으로 동작시킨다.
		swapChainDesc.BufferCount = 1;

		// 사용 용도를 백버퍼로 설정
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

		// 렌더링을 하기 위해, 렌더링할 창의 핸들을 가져온다.
		swapChainDesc.OutputWindow = hwnd;

		// Turn multisampling off.
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;

		// 전체화면, 창화면에 대한 설정
		if (fullscreen) swapChainDesc.Windowed = false;
		else swapChainDesc.Windowed = true;

		// 버퍼의 교체 방식의 세팅 값인데 지금은 단일 버퍼라 의미 없음.
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		// 플래그를 설정하지 않는다 = 일단 중요하지 않다.
		swapChainDesc.Flags = 0;

		// 버퍼(도화지)의 크기 설정
		swapChainDesc.BufferDesc.Width = screenWidth;
		swapChainDesc.BufferDesc.Height = screenHeight;

		// 버퍼의 픽셀 형식 설정
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		// 버퍼를 위에서부터 차례대로 그릴건지, 짝수 줄부터 그릴건지, ..등에 대한 세팅, 튜토리얼에서는 기본값 사용.
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

		// 스케일링에 대한 설명, 튜토리얼에서는 기본값 사용.
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		// 수직 동기화 여부에 따라 주사율을 설정한다.
		if (m_vsync_enabled)
		{
			swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
			swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
		}
		else
		{
			// 수직 동기화가 아니라면 주사율의 제한을 없앤다.
			swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
			swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		}

		// 사용할 DirectX의 버전을 정의
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

		// 5.2. 위에서 세팅한 DXGI_SWAP_CHAIN_DESC를 이용해 스왑체인, Direct3D device, Direct3D device context를 생성한다.
		result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
			D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	}

	// 6. RenderTargetView 생성
	{
		// 6.1. 스왑체인의 0번째 버퍼 포인터를 가져온다.
		// backBufferPtr : 생성된 버퍼의 포인터
		ID3D11Texture2D* backBufferPtr;
		result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
		if (FAILED(result)) return false;


		// 6.2. 렌더 타겟 뷰를 백버퍼와 연결하면서 생성한다.
		result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
		if (FAILED(result)) return false;


		// 6.3. 이제 필요 없는 메모리를 해제한다.
		backBufferPtr->Release();
		backBufferPtr = 0;
	}
	
	
	// 7. Depth Stencil Buffer 생성
	D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
	ZeroMemory(&depthStencilBufferDesc, sizeof(depthStencilBufferDesc));
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	{
		// 7.1. 픽셀의 depth값과 Stencil값을 담을 버퍼 Desc 초기화
		depthStencilBufferDesc.Width = screenWidth;
		depthStencilBufferDesc.Height = screenHeight;
		depthStencilBufferDesc.MipLevels = 1;
		depthStencilBufferDesc.ArraySize = 1;
		// 32bit중 24bit는 Depth buffer로, 8bit는 Stencil buffer로 사용.
		depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilBufferDesc.SampleDesc.Count = 1;
		depthStencilBufferDesc.SampleDesc.Quality = 0;
		depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		// 사용 용도는 Depth stencil
		depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilBufferDesc.CPUAccessFlags = 0;
		depthStencilBufferDesc.MiscFlags = 0;

		result = m_device->CreateTexture2D(&depthStencilBufferDesc, NULL, &m_depthStencilBuffer);
		if (FAILED(result)) return false;


		// 7.3. depth, stencil이 어떻게 작동될지에 대한 규칙에 대한 Desc 초기화
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;

		// 픽셀이 전면을 향하고 있는 경우 스텐실 작업. 
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// 픽셀이 후면을 향하고 있는 경우 스텐실 작업. 
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;


		// 7.4. depth stencil state 생성
		result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
		if (FAILED(result)) return false;


		// 7.5. depth stencil state 등록
		m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);


		// 7.6. Depth stencil view DESC 초기화
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;


		// 7.7 Depth stencil view 생성
		result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
		if (FAILED(result)) return false;


		// 렌더 타겟 뷰와 깊이 스텐실 버퍼를 출력 렌더 파이프라인에 바인딩한다.
		m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
	}

	
	// 폴리곤이 어떻게 그려질지의 속성을 Desc에 채워넣기
	D3D11_RASTERIZER_DESC rasterDesc;
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

	// Desc를 이용하여 rasterizer state를 생성
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(result)) return false;

	// rasterizer state 파이프 라인 RS단계에 지정
	m_deviceContext->RSSetState(m_rasterState);

	// Viewport는 다른 그래픽 리소스와 달리 Desc없이 직접 설정한다.
	// 뷰포트 속성 설정
	m_viewport.Width = (float)screenWidth;
	m_viewport.Height = (float)screenHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;

	// viewport를 생성하고 파이프 라인 RS단계에 지정.
	m_deviceContext->RSSetViewports(1, &m_viewport);


	// 3D장면을 2D로 변환하는데 사용되는 투영 행렬을 만든다.
	// 셰이더에 전달하여  씬을 렌더링 할 수 있다.
	fieldOfView = 3.141592654f / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	// Initialize the world matrix to the identity matrix.
	m_worldMatrix = XMMatrixIdentity();

	// Create an orthographic projection matrix for 2D rendering.
	m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	// Create the projection matrix for 3D rendering.
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
		m_rasterState = 0;
	}

	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	if (m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}

	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if (m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = 0;
	}

	if (m_device)
	{
		m_device->Release();
		m_device = 0;
	}

	if (m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = 0;
	}

	return;
}

void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];

	// 매개변수로 들어온 색상으로 버퍼를 초기화 한다.
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