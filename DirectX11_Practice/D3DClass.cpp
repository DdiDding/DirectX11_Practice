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
	// DXGI 개체를 생성하고 관리하는 기능을 한다.
	IDXGIFactory* factory;
	// IDXGIAdapter : 그래픽 카드의 특정 기능과 정보를 관리하는 인터페이스
	// 그래픽 카드의 정보가 나열 되고 해당 그래픽 카드와 관련된 작업을 수행 가능
	IDXGIAdapter* adapter;
	// IDXGIOutput : 그래픽 카드의 출력장치(모니터, ...)의 정보를 관리하는 인터페이스
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


	// 수직 동기화 세팅
	m_vsync_enabled = vsync;

	//////////////////////////////////////////////////////////////////////////////////////////////
	// 주사율 세팅 값 알아내기
	//////////////////////////////////////////////////////////////////////////////////////////////

	// DirectX graphics Infrastructure(DXGI) factory를 생성
	// DXGI는 일단 프로그램과 그래픽 기능 장치(모니터, 그래픽카드 ..) 사이에서 일해주는 기능이라고 생각하자.
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);

	if (FAILED(result))
	{
		return false;
	}

	// 방금 만든 DXGI factory를 사용해 0번째 사용하는 그래픽카드의 IDXGIAdapter를 생성한다.
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	// IDXGIAdapter를 이용해 그래픽 카드의 사용 가능한 0번째 출력장치(모니터)의 정보를 저장한다.
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	// 색상 형식이 DXGI_FORMAT_R8G8B8A8_UNORM인 디스플레이 모드의 종류 개수를 numModes에 저장한다.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// 방금 저장한 디스플레이 모드의 종류 개수 만큼 배열을 생성한다.
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	// numModes의 수 만큼 디스플레이 모드 정보를 displayModeList으로 가져온다.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
	{
		return false;
	}

	// 디스플레이 모드 목록에서 모니터의 크기에 맞는 디스플레이 모드를 찾는다.
	// 일치하는 디스플레이 모드를 찾으면 주사율의 분자 분모를 저장한다.
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
	// 그래픽 카드 이름과 vram 용량 알아내기
	//////////////////////////////////////////////////////////////////////////////////////////////
	
	// DXGI_ADAPTER_DESC adapterDesc;
	// 그래픽 카드의 설명을 가져온다.
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}

	// dedicated video card memory(VRAM)를 메가바이트 단위로 저장합니다.
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// 그래픽 카드의 이름을 Char형 배열로 변환하여 저장합니다.
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
	// 스왑 체인
	//////////////////////////////////////////////////////////////////////////////////////////////

	// 스왑체인의 세팅값(DXGI_SWAP_CHAIN_DESC)을 설정하기전 0으로 초기화 한다.
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// 버퍼를 보통 2개, 혹은 3개로 설정하지만 일단 튜토리얼에서는 버퍼 개수를 1개, 싱글 버퍼링으로 동작시킨다.
	swapChainDesc.BufferCount = 1;

	// 버퍼(도화지)의 크기 설정.
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// 버퍼(도화지)의 픽셀 형식을 설정.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// 버퍼의 주사율을 설정, 수직 동기화 여부에 따라 분기가 갈린다.
	if (m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	// 수직 동기화가 아니라면 주사율의 제한이 없어진다.
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// 버퍼의 용도를 설정하는건데 일단 튜토리얼에서는 렌더링할 목적으로 설정했다.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// 버퍼의 핸들을 설정.
	swapChainDesc.OutputWindow = hwnd;

	// 멀티 샘플링(안티 에일리어싱) 을 끈다.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// 전체 화면의 설정 분기.
	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// 버퍼를 위에서부터 차례대로 그릴건지, 짝수 줄부터 그릴건지, ..등에 대한 세팅, 튜토리얼에서는 기본값 사용.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	// 스케일링에 대한 설명, 튜토리얼에서는 기본값 사용.
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// 버퍼의 교체 방식의 세팅 값인데 지금은 단일 버퍼라 의미 없음.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// 플래그를 설정하지 않는다 = 일단 중요하지 않다.
	swapChainDesc.Flags = 0;

	// 사용할 DirectX의 버전을 정의
	featureLevel = D3D_FEATURE_LEVEL_11_0;
	
	// 여태 세팅한 DXGI_SWAP_CHAIN_DESC를 이용해 스왑체인, Direct3D device, Direct3D device context를 생성한다.
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
			D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	if (FAILED(result))
	{
		return false;
	}

	// 스왑체인의 버퍼 포인터를 가져온다.
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
	{
		return false;
	}

	// 렌더 타겟 뷰(실제 버퍼)를 버퍼를 연결하면서 생성한다.
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	// 이제 이용가치가 사라진 버퍼의 포인터를 메모리 해제한다.
	backBufferPtr->Release();
	backBufferPtr = nullptr;
	
	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// 버퍼는 결국 2D 텍스쳐일 뿐이기에 버퍼로 사용할 2D 텍스쳐부터 만들것이다.
	// Set up the description of the depth buffer.
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	// 32bit중 24bit는 Depth buffer로, 8bit는 Stencil buffer로 사용한다는 뜻이다.
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;
	
	// 하나의 2D Texture를 깊이, 스텐실 버퍼로 사용한다.
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
	

	// depth stencil state 생성
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result))
	{
		return false;
	}

	// depth stencil state 등록
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	// depth stencil view 초기화
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// depth stencil view Desc 설정
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// depth stencil view 생성
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
	{
		return false;
	}

	// 렌더 타겟 뷰와 깊이 스텐실 버퍼를 출력 렌더 파이프라인에 바인딩합니다. 
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
	Now we will create the projection matrix.The projection matrix is used to translate the 3D scene into the 2D viewport space that we previously created.We will need to keep a copy of this matrix so that we can pass it to our shaders that will be used to render our scenes.

		// Setup the projection matrix.
		fieldOfView = 3.141592654f / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	// Create the projection matrix for 3D rendering.
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);
	We will also create another matrix called the world matrix.This matrix is used to convert the vertices of our objects into vertices in the 3D scene.This matrix will also be used to rotate, translate, and scale our objects in 3D space.From the start we will just initialize the matrix to the identity matrix and keep a copy of it in this object.The copy will be needed to be passed to the shaders for rendering also.

		// Initialize the world matrix to the identity matrix.
		m_worldMatrix = XMMatrixIdentity();
	This is where you would generally create a view matrix.The view matrix is used to calculate the position of where we are looking at the scene from.You can think of it as a camera and you only view the scene through this camera.Because of its purpose I am going to create it in a camera class in later tutorials since logically it fits better there and just skip it for now.

		And the final thing we will setup in the Initialize function is an orthographic projection matrix.This matrix is used for rendering 2D elements like user interfaces on the screen allowing us to skip the 3D rendering.You will see this used in later tutorials when we look at rendering 2D graphics and fonts to the screen.

		// Create an orthographic projection matrix for 2D rendering.
		m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	return true;
}
The Shutdown function will release and clean up all the pointers used in the Initialize function, it's pretty straight forward. However before doing that I put in a call to force the swap chain to go into windowed mode first before releasing any pointers. If this is not done and you try to release the swap chain in full screen mode it will throw some exceptions. So, to avoid that happening we just always force windowed mode before shutting down Direct3D.

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