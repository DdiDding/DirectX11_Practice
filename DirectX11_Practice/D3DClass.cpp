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

D3DClass::D3DClass(const D3DClass& other)
{
}

bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{
	HRESULT result;
	
	unsigned int i;
	int error;
	
		D3D11_RASTERIZER_DESC rasterDesc;
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
	// 버퍼는 TEXTURE2D형태로 접근할 수 있다.
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	{

	}

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;

	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	// // 32bit중 24bit는 Depth buffer로, 8bit는 Stencil buffer로 사용.
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	// 사용 용도는 Depth stencil
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;


	// depthBufferDesc를 이용해 
	// Create the texture for the depth buffer using the filled out description.
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

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;


	// Create the depth stencil state.
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result))
	{
		return false;
	}

	// Set the depth stencil state.
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);


	// Initialize the depth stencil view.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
	{
		return false;
	}

	// Bind the render target view and depth stencil buffer to the output render pipeline.
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);


	// Setup the raster description which will determine how and what polygons will be drawn.
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

	// Create the rasterizer state from the description we just filled out.
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(result))
	{
		return false;
	}

	// Now set the rasterizer state.
	m_deviceContext->RSSetState(m_rasterState);


	// Setup the viewport for rendering.
	m_viewport.Width = (float)screenWidth;
	m_viewport.Height = (float)screenHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;

	// Create the viewport.
	m_deviceContext->RSSetViewports(1, &m_viewport);


	// Setup the projection matrix.
	fieldOfView = 3.141592654f / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	// Create the projection matrix for 3D rendering.
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	// Initialize the world matrix to the identity matrix.
	m_worldMatrix = XMMatrixIdentity();

	// Create an orthographic projection matrix for 2D rendering.
	m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	return true;
}